/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmedeiro <rmedeiro@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 14:44:17 by rmedeiro          #+#    #+#             */
/*   Updated: 2025/09/15 01:58:44 by rmedeiro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


> pipe(): Um pipe é um canal unidirecional, criado pelo kernel, com o fim de ligar/encadear 
          processos - ele liga um processo escritor (que escreve os bytes numa ponta) a um 
          processo leitor (que lê esses bytes na noutra ponta). 
		  Funciona como um tubo de fluxo de bytes: escreve-se na extremidade de escrita (p[1]) e
		  lê-se na extremidade de leitura (p[0]).

		  O pipe é usado para ligar o output (stdout) de um processo ao input (stdin) de outro processo.
          
          O pipe é unidirecional, pois escrevemos no write end e lê-mos do read end.

		  Um pipe existe enquanto houver pelo menos refererencias abertas a qualquer uma das pontas (read ou write) 
		  em algum processo.
		  Assim que estiverem fechadas todas as referencias a ambas as pontas (read e write) em todos os processos, 
		  o kernel destrói o pipe e liberta o buffer.
		  
          > Alocação e criação do pipe
          
            Quando chamamos a função pipe(int p[2]), o kernel cria um objeto pipe com um buffer e abre duas pontas
            desse pipe: 
            
                - p[0]: read end -> aberto só para leitura do pipe: associado ao FD de leitura;
                - p[1]: write end -> aberto só para escrita no pipe: associado ao FD de escrita;
            
            Assim, o pipe() devolve dois FDs novos que apontam para as duas pontas do mesmo pipe.

		  > O que é a pipeline ?

		    A pipeline é uma cadeia de processos ligados por pipes, onde o output (stdout) de um processo de 
			cada comando liga-se ao input (stdin) do processo seguinte.
			Ou seja, o stdout de cada comando vai para o stdin do próximo comando.
			
			A pipeline permite que o output de um comando seja diretamente usado como input de outro comando,
			sem necessidade de ficheiros intermédios.

			Exemplo: cmd1 | cmd2 | cmd3

			Neste exemplo, o output do cmd1 é passado diretamente como input para o cmd2 através de um pipe.
			O output do cmd2 é passado como input para o cmd3 através de outro pipe.
			O resultado final é que o output do cmd3 é o resultado combinado dos três comandos.
			
			Vejamos um exemplo: ./pipex_bonus Makefile  cat  "grep 'SRCS'"  "wc -l"  outfile

			Os três processos executam “em paralelo”… mas com dependências

			Os processos executam concorrentemente, mas há uma ordem parcial imposta pelos pipes (dependência de dados):

			Quem costuma terminar primeiro? Normalmente o comando cat.
			Ele só precisa de ler o Makefile inteiro e escrever no write end do pipe A. 
			Quando acaba de escrever e fecha o stdout, o write end do pipe A fecha → o comando grep recebe o EOF quando 
			consumir tudo que estava no buffer e então termina.

			Quem termina a seguir? Em geral, grep 'SRCS'.
			Ele não pode acabar antes do cat fechar o pipe (precisa do EOF para saber que não virá mais dados). 
			Quando termina, fecha o write end do pipe B → o comando wc recebe o EOF ao consumir o que restou e termina.

			Quem termina por último? Tipicamente, wc -l.
			Ele só pode imprimir a contagem final depois do grep fechar o stdout (pipe B). 
			Por isso o último comando costuma ser o último a sair. 
			O código de saída do pipeline (no shell e no teu wait_processes) é o exit status do último comando (wc).

			Resumo da ordem (habitual): cat → grep → wc.
			Eles são concorrentes, mas cada um depende do EOF do anterior para concluir.

			“Independentes” vs “dependentes por dados”

			 Processos independentes: cada comando executa o seu próprio processo (PID diferente). 
			                          O pipeline pode executar todos “ao mesmo tempo”. Se um falhar, não termina automaticamente os outros.

			 Dependentes por dados (pipeline): apesar de independentes, estão ligados por pipes. O stdout de um é o stdin do seguinte. 
			                                   Se o de cima não escrever nada (ou nem chegar a abrir o write end), o de baixo ainda corre, mas 
											   recebe o EOF imediato e termina assim que consumir (que é “nada”).

			 O EOF propaga-se quando todas as write ends do pipe estão fechadas.
			 Se o primeiro comando não escrever, mas fecha corretamente o write end, os seguintes não bloqueiam (não ficam à espera): os seguintes
			 lêem o EOF e acabam rápido.
			 Se o write end não for fechado (ex.: ficar aberto no parent), o leitor pode bloquear à espera de dados que nunca chegam — por isso é 
			 crucial fechar as pontas não usadas em todos os processos (pai e filhos).
			

			> O que é um FD e como fica aberto?

            Um file descriptor (FD) é um número inteiro (0, 1, 2, 3, ...) dentro de um processo, que identifica um objeto aberto no kernel.
            Cada processo tem uma tabela de FDs. O número FD é um índice/posição nessa tabela. 
            Cada indice/posição da tabela aponta para um FD aberto, no kernel, que por sua vez aponta para um objeto.

                                        Processo
                ┌---------------------------------------------------┐
                │ Tabela de FDs (por processo)                      │
                │  FD 0 ──► file desc A ──► objeto (tty: stdin)     │
                │  FD 1 ──► file desc B ──► objeto (tty: stdout)    │
                │  FD 2 ──► file desc C ──► objeto (tty: stderr)    │
                │  FD 3 ──► file desc D ──► objeto (pipe: read end) │
                │  FD 4 ──► file desc E ──► objeto (pipe: write end)│
                └---------------------------------------------------┘
                
            Os stdin (0), stdout (1), stderr (2) são apenas convenções (constantes). 
            São FDs “normais”, só que com significado especial por convenção.

            Um FD aparece quando chamamos uma syscall que abre/cria algo: - open(...) → devolve um novo FD aberto;
                                                                          - pipe(p) → devolve dois FDs abertos (p[0] e p[1]);
                                                                          - dup/dup2 → duplica um novo FD a apontar para o mesmo objeto;
                                                                          - fork() → o filho herda os FDs abertos do pai.

            Um FD está aberto enquanto existir uma posição válida na tabela do processo a apontar para um FD que por sua vez aponta para 
            um objeto e mantém uma referência com esse objeto.

            Assim, quando abrimos algo, o kernel devolve esse inteiro (fd). 
			Por exemplo, no pipe(int p[2]), o kernel cria um objeto pipe e devolve dois FDs abertos (p[0] e p[1]).
    
          > Como um FD se fecha ?

            Um FD é fechado com a função close(fd), que remove a posição desse FD da tabela de FDs do processo.
            Se não houver mais FDs (em qualquer processo) a apontar para um FD ou ponta, o kernel liberta o buffer.
            Assim, o objeto no kernel (neste caso, o pipe) só é destruído quando todas as referencias (em todos os processos) forem fechadas.
              
            Para o caso de terminar o processo, o kernel fecha automaticamente todos os FDs desse processo (como se chamasse o close() para
            cada um).
                               
          > O que são referências ?

            Referência significa que há algo no sistema a apontar para tal coisa.
            
            Desta forma, o kernel mantém os contadores separados: - número de leitores: corresponde a quantos FDs 
                                                                    (em todos os processos) ainda apontam para a read end.
                                                                  - número de escritores: corresponde a quantos FDs (tm todos
                                                                    os processos) ainda apontam para a write end.

            Assim, os contadores de referencias de leitores/escritores correspondem a quantos FDs (no sistema inteiro) ainda 
            apontam para cada ponta do pipe.
                                                                    
          > Como as referências aparecem ?

            Ao criarmos o objeto pipe - pipe() - são devolvidos dois FDs que apontam para as duas pontas do pipe:

                - p[0]: read end -> incrementa o número de leitores para 1.
                - p[1]: write end -> incrmeenta o número de escritores para 1.
                
            Como vamos ver depois, com o fork(), o child herda os FDs do parent process. Assim cada ponta (read e write ends)
            tem mais uma referência:
            
                - número de leitores incrementa para mais um (o child tem um p[0] próprio).
                - número de escritores incrementa para mais um (o child tem um p[1] próprio).

            O dup2 cria também outra referência que aponta para a read end/write end.

            > Como as referências deixam de existir ?

              Ao fazermos close(fd) em qualquer processo: - Se o fd era a read end -> decrementa o número de leitores.
                                                          - Se o fd era a write end -> decrementa o número de escritores.
                                                          
              Também ao fazermos execve() nesse FD, o FD é fechado automaticamente na troca de imagem -> decrementa o contador.
              
              Ao terminarmos o processo, por exemplo, com o exit, o kernel fecha todas as FDs abertas desse processo, aplicando
              os decrementos dos contadores de uma vez só.
            
            > Ciclo do pipe
            
              Podemos assumir, assim, que o pipe existe enquanto houver pelo menos uma ponta aberta (read ou write) em algum processo,
              ou seja, enquanto houver pelo menos uma referencia aberta em algum processo (número de leitores e escritores > 0).
          
              QUando todas as referencias a ambas as pontas são fechadas (close em todos os FDs de leitura e de escrita, em 
              todos os processos), o número de referencias aos leitores e escritores chega a zero e o kernel destrói o pipe (liberta o buffer).

              > Leitura - O read end p[0] corresponde à ponta de leitura e lê com o read(p[0], buf, n):  
          
                - Se houver dados no buffer, o read devolve os bytes disponiveis (até n bytes).
                - Se não há dados no buffer (se o buffer está vazio) mas ainda existe algum write end p[1] aberto, o read 
                  bloqueia (fica à espera de dados).
                - Se todos os write ends p[1] foram fechados, o read devolve 0 (EOF), sinal de que não virá mais nada. Acabou o fluxo.
            
              > Escrita - O write end p[1] corresponde à ponta de escrita e é utilizado com write (p[1], buf, n):
            
                - Se houver espaço no buffer, o write escreve e devolve quantos bytes conseguiu escrever.
                - Se o buffer do pipe estiver cheio mas ainda existe algum read end p[0] aberto, o write bloqueia (fica à espera) até o recetor ler
                  e libertar espaço (o recetor liberta espaço ao ler).
                - Se não existe nenhum read end aberto (todos os p[0] fechados), o write falha e o processo recebe SIGPIPE (por omissão termina).



			
	int main(int ac, char **av, char **envp)
	{
		int prev_read_fd;
		int last_pid;
		int exit_code;
		int i;

		if (ac < 5)
			show_usage_exit2();
		if (ft_strncmp(av[1], "here_doc", 8) != 0)
		{
			first_child(av, &prev_read_fd, envp);
			i = 3;
			while (i < ac - 2)
			{
				last_pid = middle_child(av, &prev_read_fd, envp, i);
				i++;
			}
			last_pid = last_child(ac, av, prev_read_fd, envp);
		}
		else
			last_pid = ft_heredoc_pipeline(ac, av, envp);
		exit_code = wait_processes(last_pid);
		return (exit_code);
	}


	
	void	first_child(char **av, int *prev_readfd, char **envp)
	{
		int	infile_fd;
		int pipefd[2];
		pid_t pid;
		
		get_pipe_and_fork(pipefd, &pid);
		if (pid == 0)
		{
			close(pipefd[0]);
			infile_fd = open(av[1], O_RDONLY);
			if (infile_fd == -1)
			{
				close(pipefd[1]);
				error_exit("Error Opening Input File!");
			}
			safe_dup2(infile_fd, STDIN_FILENO);
			safe_dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			close(infile_fd);
			ft_exec_cmd(av[2], envp);
		}
		close(pipefd[1]);
		*prev_readfd = pipefd[0];
	}

	a1) Primeiro chamamos a função get_pipe_and_fork() para criar o pipe e o criar o primeiro processo (o primeiro comando).

	      get_pipe_and_fork(int pipefd[2], pid_t *pid)

		  O kernel cria um objeto pipe com um buffer e devolve dois FDs abertos que apontam para as duas pontas do mesmo pipe:
	      
            - pipefd[0]: read end -> aberto só para leitura do pipe: associado ao FD de leitura;
            - pipefd[1]: write end -> aberto só para escrita no pipe: associado ao FD de escrita;

		  Tabela de FDs (no parent) logo após pipe:

		  FD 0 (STDIN)  -> aponta para o terminal/ficheiro
		  FD 1 (STDOUT) -> aponta para o terminal/ficheiro
	      FD 2 -> stderr
	      FD 3 -> aponta para o pipefd[0]  (read end)
	      FD 4 -> aponta para o pipefd[1]  (write end)
            
          O fork() cria um novo processo (child 1) que é uma cópia do parent.
		  O child herda a mesma tabela de FDs do parent, ou seja, o child 1 também tem o pipefd[0] e o pipefd[1] abertos.

	  	  Parent (após o fork):  FD 3 -> pipefd[0]
						         FD 4 -> pipefd[1]

		  Child 1 (cópia):  FD 3 -> pipefd[0]
					        FD 4 -> pipefd[1]
		  
		  O fork() devolve o PID do child no parent (pid > 0) e devolve 0 no pid do child (pid == 0).

		  Por que criar o pipe antes do fork?
		  
		  Para que tanto o parent quanto o child possuam os FDs do pipe e cada um feche o que não usa. 
		  Isso é essencial para a correta sinalização do EOF e para evitar deadlocks.

	 a2) Se pid == 0, entramos no child 1 (que vai ser o nosso cmd1).
	     Começamos por fechar o read end do pipe (pipefd[0]) neste processo, pois este processo não vai ler do read end do pipe, só 
		 vai escrever no write end do pipe (o processo que vai ler (o leitor) vai ser o comando seguinte a este).

		 O kernel mantém os contadores: o número de pontas de leitura que estão abertas (pipefd[0]) e o número de pontas de
		 escrita que estão abertas (pipefd[1]) do pipe.
		 Se o processo que vai escrever no pipe (o escritor, que neste caso é o child 1) mantiver aberta a read end (pipefd[0]), então o 
		 número de referencias ao read end será maior do que 0 (> 0) - ou seja, o número de pontas de leitura será > 0 - mesmo que o 
		 verdadeiro processo que vai ler do pipe (o leitor, que é o child 2) já tenha fechado/morrido.
		 Assim, quando o processo leitor real fechar, o processo escritor continua a escrever até encher o buffer, e então bloqueia
		 para sempre, pois ninguém está a consumir (ler).

		-> Fechar o read end (pipefd[0]) no child 1 faz com que o número de pontas de leitura abertas (pipefd[0]) seja a correta,
		   assim, quando o child 2 morrer, esse número será 0 e write no child 1 já não bloqueia.
		 
		-> Ao fechar o read end, garantimos que o contador de referências do read end do pipe está correto.
		
	
	 a3) infile = open(av[1], O_RDONLY);

	     Aqui, vamos abrir o ficheiro infile (av[1]) para lermos a partir dele o conteúdo que será a entrada do child 1.

	     A função open() abre um ficheiro e devolve um FD. 
		 O kernel vai percorrer o ficheiro (av[1]) e verifica, com o O_RDONLY, se esse ficheiro tem permissão só de leitura. 
	     Se tudo estiver OK, é devolvido um FD novo (um número inteiro >= 0) que será armazenado no infile_fd.

	     Se o open() falhar ao abrir o ficheiro infile (-1) - ou por um ficheiro inexistente, permissão negada, etc - o child 1 não irá 
		 conseguir ligar/redirecionar o STDIN para o FD do infile e não vai conseguir executar o child 1 com sucesso.
		 Portanto, fechamos o write end do pipe (pipefd[1]) no child, pois se não fechassemos o pipefd[1] aqui, o processo leitor (o child 2)
		 poderia ficar bloqueado à espera de ler mais dados do read end do pipe ou do EOF (pois haveria ainda um escritor aberto, o pipefd[1] do child 1), mesmo que 
		 o parent e o child 1 já tivessem morrido).
		 Saímos com o error_exit() para terminar o processo com o código de erro apropriado.

	     Porquê fechar aqui? 

		 Como vimos, o objeto pipe no kernel tem contadores: - o número de referencias para o read end (quantas pontas de
															   leitura estão abertas em todos os processos).
															 - o número de referencias para o write end (quantas pontas de 
															   escrita estão abertas em todos os processos).

		Se o child 1 morrer/terminar sem fechar, continuaria a existir (durante algum tempo) um pipefd[1] vivo, assim, o processo
		leitor (child 2) pode não ver o EOF quando devia e ficaria bloqueado à espera de mais dados.

		> O que acontece no pipeline quando o infile não existe ?
		
		  No caso: ./pipex infile_inexistente cat ls outfile

		  O kernel cria o pipe antes de fazer o fork().
		  
		    - Fd de leitura do pipe (pipefd[0]) -> vai para o stdin do segundo comando (ls).
			- Fd de escrita do pipe (pipefd[1]) -> vai para o stdout do primeiro comando (cat).

		  O fork() cria o child 1 (cat) e o child 2 (ls) que herdam os FDs do parent (incluindo o pipefd[0] e o pipefd[1]).
	
		  Entramos no child 1 (pid == 0) e redirecionamos o stdin (FD 0) para o infile (infile_fd) e o stdout (FD 1) para o pipe (pipefd[1]).
		  O child 1 ao tentar abrir o infile, o open() falha (infile não existe), devolve -1 e imprime o erro e termina sem escrever nada no pipe.
		  O child 1 fecha o pipefd[1] e termina sem escrever nada no pipe e sem executar.

		  Entramos no child 2 (pid == 0) e redirecionamos o stdin (FD 0) para o pipe (pipefd[0]) e o stdout (FD 1) para o outfile (outfile_fd).
		  O child 2 executa normalmente.
		  O child 2 tenta ler do pipe (pipefd[0]), mas o pipe está vazio (o cat não escreveu nada, pois o infile não existe).
		  Como o read end do pipe (pipefd[0]) do child 1 foi fechado (o cat saiu sem escrever nada), o read() do child 2 (ls) vê logo o EOF e termina
		  sem ler nada, mas escreve o seu output no outfile.
		  Ou seja, o segundo comando executa mesmo que o primeiro tenha falhado a abrir o ficheiro infile.
		  O segundo comando (ls) não depende do primeiro (cat) ter sucesso, pois os processos são independentes e ocorrem em paralelo.
		  Ele apenas não recebe/lê nada do pipe (o child 1 passou 0 bytes para o pipe), por isso produz um output normal (o output do ls) correspondente a 
		  uma entrada vazia (0 bytes) no outfile.
		  O ficheiro outfile do último comando (o ls) é criado/truncado normalmente pelo próprio processo do segundo comando (ls), independentemente do
		  primeiro comando (cat) ter falhado ou não. 
		  Portanto o outfile existe mesmo que o primeiro tenha falhado.

		  Como os processos são criados em paralelo e são independentes, a falha em abrir o infile afeta apenas o processo do primeiro comando (cat), que
		  termina sem executar e saí com o erro, mas não impede o segundo comando (ls) de executar e criar o outfile, pois ele já tem o seu stdin redirecionado 
		  para read end do pipe (pipefd[0]) e já está em execução e lê do pipe o que houver (neste caso, nada, pois o cat não escreveu nada).


	 a4) safe_dup2(int oldfd, int newfd)

	     A função dup2(int oldfd, int newfd), tem como objetivo fazer com o que o newfd passe a apontar exatamente para o mesmo 
	     objeto do que o oldfd.
	     Ou seja, o FD do newfd passa a apontar para o mesmo alvo (o mesmo ficheiro/pipe,...) que o FD do oldfd.
	     Em caso de sucesso, devolve o newfd.

		 O dup2() não copia bytes, ele duplica a ligação do oldfd, para o mesmo objeto aberto do kernel, utilizando um FD diferente,
		 o newfd.
		 
		 O dup2(), no caso de o newfd já estiver aberto, ele fecha-o antes de fazer a ligação. Assim, se o oldfd != newfd e newfd estiver
		 aberto, o kernel fecha o newfd (dentro da mesma syscall).
	     De seguida, redireciona/aponta o newfd para o mesmo objeto que o oldfd aponta.
	     Assim, newfd e oldfd passam a apontar para o mesmo objeto.

	     O dup2 é assim uma ferramenta para redireções (ligar stdin/stdout a pipes/ficheiros).

		 Cada ponta do pipe tem uma função diferente: -> write end (pipefd[1]): ponta do pipe onde escrevemos.
													  -> read end (pipefd[0]): ponta do pipe onde se lê.

		 Com estes dois FDs, ligamos o output (o STDOUT) de um processo ao input (ao STDIN) de outro ao usar a função dup2() e fechamos os 
		 FDs que não usamos.

		 dup2(int FD_origem, int FD_destino): 1) Se o FD_destino estiver aberto, o kernel fecha-o (exceto quando FD_origem == FD_destino, não faz nada, e não fecha).
											  2) Depois, faz o FD_destino apontar para o mesmo objeto que o FD_origem aponta.
												 Ficamos com duas referencias para o mesmo objeto.
											  3) Não copia bytes, só copia a ligação.
											  4) Devolve o FD_destino.

		  Consequência: POr exemplo, após dup2(infile_fd, STDIN_FILENO), tanto o STDIN (FD 0) como o FD do infile apontam para o mesmo infile_fd (há assim duas referencias).
						Por isso, teremos de fechar o infile_fd e ficamos só com o FD 0 como referência.

		  Genericamente, depois do dup2(): - dup2(pipefd[1], STDOUT) → o FD 1 (stdout) passa a escrever no write end (pipefd[1]) do pipe.
					                       - dup2(p[0], STDIN) → o FD 0 (stdin) passa a ler do read end (pipefd[0]) do pipe.

		  A função dup2() não cria um stdin/stdout novos, ela apenas aponta os FDs 0 ou 1 para a mesma ponta do pipe (read ou write end) ou para o mesmo ficheiro, aumentando
		  temporariamente a referencia até fecharmos os FDs antigos.

		  Porquê usar a função dup2() ?

		  Os programas não sabem que existe um pipe: eles escrevem na sua saída padrão (FD 1) e lêem da entrada padrão (FD 0).
		  Para qualquer comando funcionar normalmente, ele deve continuar a utilizar o seu stdin/stdout.
		  A função dup2() é a forma correta de redirecionar os FDs padrão (0, 1, 2) para outros FDs.
		  O dup2(oldfd, newfd) faz com que o newfd passe a apontar para o mesmo objeto que o oldfd aponta.
		  Assim, redirecionamos o stdin/stdout para onde queremos.
				
		  Porquê fechar o read end/write end (p[0]/p[1]) depois do dup2 ?

		  > Garantir o EOF correto: O recetor (o read()) do read end p[0] só vê o EOF quando todas as referencias ao write end (p[1]) estiverem fechadas em todos os processos
									(incluindo o parent process). Se tiverem fechadas, o próximo read() devolve 0.
									Se esquecermos de um  write end p[1] aberto no parent process (ou noutro processo) por engano, o read end p[0] nunca chega a ver o EOF
									e pode ficar bloqueado para sempre à espera de dados que não virão.
			
		  > Evitar FDs a mais sem utilidade: Depois do dup2, o FD 1 (ou o FD 0) apontam para o pipe. Manter o p[0] ou o p[1] abertos, duplica as referências e
 											 pode alterar a semântica.
			
		  > Se não existe nenhum read end aberto (ou seja, todos os p[0] estão fecharados), o write falha e o processo recebe 
			SIGPIPE (por omissão termina).


		  > safe_dup2(infile, STDIN_FILENO)

		    Se o infile for um FD válido, o kernel fecha o STDIN (FD 0) (se estiver aberto) e faz o FD 0 apontar para o mesmo FD do infile.
	        Por exemplo, se o FD do infile é 3, com o dup2(), o FD 0 passa apontar para o FD 3, que é o FD do infile.

            No primeiro child, vamos ligar/redirecionar o STDIN (FD 0) ao FD do infile (infile_fd).
		    Assim, o STDIN do child 1 que antes apontava e lia do terminal, passa a ler do infile_fd (o ficheiro av[1]).

			Resultado: o stdin passa a ler do infile, pois o stdin (FD 0) aponta agora para o FD do infile (FD 0 -> infile_fd)

			Porquê que fechamos ?

			Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
			O FD infile já não será utilizado.
			Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
			Se não fecharmos o FD do infile e o processo sair por erro mais tarde, durante esse intervalo, o kernel
			continua a ver que ainda há um infile_fd aberto.
			Se outro processo abrir o mesmo ficheiro, pode receber o mesmo FD (o mesmo número inteiro) e causar confusão.
			Ao fechar imediatamente o infile_fd no erro, o número de referencias ao infile é decrementado.

		  > safe_dup2(pipefd[1], STDOUT_FILENO)

			O kernel fecha o STDOUT (FD 1) (se estiver aberto) e faz o STDOUT (FD 1) apontar para o mesmo FD que o write end do pipe.
			Por exemplo, se o FD do write end for 5, com o dup2(), o FD 1 passa a apontar para o FD 5, que é o FD do write end do pipe.

			No primeiro child, vamos ligar/redirecionar o STDOUT (FD 1) ao FD do write end do pipe (pipefd[1]).
			Assim, o STDOUT do child 1 que antes apontava e escrevia no terminal, passa a escrever no write end do pipe.

			Resultado: o stdout passa a escrever no pipe, pois o stdout (FD 1) aponta agora para o FD do write end do pipe (FD 1 -> pipefd[1]).

			Porquê que fechamos ?

			Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
			O write end pipefd[1] já não será utilizado.
			Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
			Se não fecharmos o write end do pipe e o processo sair por erro mais tarde, durante esse intervalo o kernel
			continua a ver que ainda há um escritor vivo.
			O leitor (o child 2) pode não receber o EOF quando deveria, causando um bloqueio (ele fica à espera eternamente).
			Ao fechar imediatamente o pipefd[1] no erro, o número de referencias ao write end do pipe é decrementado e assim, o 
			leitor (o child 2) pode ver o EOF no timming certo (quando os restantes escritores fecharem).


	      > Comparação do antes e depois do dup2():
			
			Antes:                                               Depois:

			FD 0 (STDIN)  -> aponta para o terminal              FD 0 (STDIN)  -> aponta para o infile_fd (O stdin agora lê do infile)
			FD 1 (STDOUT) -> aponta para o terminal              FD 1 (STDOUT) -> aponta para o pipefd[1] (O stdout agora escreve no pipe)
			FD 2 (STDERR) -> aponta para o terminal              FD 2 (STDERR) -> aponta para o terminal
			FD 3 -> aponta para o infile_fd                      FD 3 -> aponta para o infile_fd (original ainda continua aberto)
			FD 4 -> aponta para o pipefd[0] [FECHADO]            FD 4 -> aponta para o pipefd[0] [FECHADO]
			FD 5 -> aponta para o pipefd[1] (write end)          FD 5 -> aponta para o pipefd[1] (original ainda aberto)

		    O infile_fd e o pipefd[1] estão agora ligados aos FDs padrão 0 e 1.
		    O infile_fd e o pipefd[1] originais ainda estão abertos, mas já não são necessários.

		 a5) close(pipefd[1]); 
	         close(infile);

	         A seguir ao dup2() fechamos o read end do pipe (pipefd[0]) e o infile_fd, porque os FDs originais já não serão precisos.

	         Como vimos, depois do dup2() ficámos com duas referências para cada destino:

			 FD 0 (stdin) ─┐                          FD 1 (stdout) ─┐
			        	   └─► [infile_fd]                           └─► [pipefd[1] do pipe]
		      infile_fd   ─┘                              pipefd[1] ─┘

		
			 Fechamos os FDs originais, pois para o child 2 ver o EOF, o kernel precisa que todas as pontas de leitura do pipe 
			 estejam fechadas. Caso ficasse o pipefd[1] aberto para além do stdout (FD 1), haveria mais uma referencia de escrita,
			 o que atrasaria o EOF do child 2 (ele só vê o EOF quando todas as referencias de escrita fecharem). Fechando o pipefd[1],
			 ficamos com apenas o stdout (FD 1) como escritor, assim, quando o processo terminar, o EOF chega ao child 2.

			 Ao fechar: - evitamos leaks.
						- reduzimos os contadores do pipe (útil para EOF no leitor),

	         A tabela de FDs final ficará:  FD 0 (STDOUT) -> aponta para o infile_fd
									        FD 1 (STDIN)  -> aponta para o pipefd[1]

											
		 a6) ft_exec_cmd(av[2], envp)

		     Finalmente, chamamos a função ft_exec_cmd() para executar o comando (cmd1) com o execve().

		
			void ft_exec_cmd(char *cmd, char **envp)
			{
				char **cmd_list;
				char *cmd_path;
				
				cmd_list = ft_parse_cmd(cmd);
				cmd_path = handle_cmd_path(cmd, cmd_list, envp);
				if (execve(cmd_path, cmd_list, envp) == -1)
				{   
					perror("execve failed");
					ft_free_str(cmd_list);
					free(cmd_path);
					if (errno == EACCES)
						exit(126);
					else if (errno == ENOENT)
						exit(127);
					exit(EXIT_FAILURE);
				}
			}

			O objetivo desta função é o de transformar o cmd (av[2]) - por exemplo: "ls -l" - num array válido, resolver o caminho do 
			executável e substituir a imagem do processo por outra (apontada pelo cmd_path), com o execve.

			1) cmd_list = ft_parse_cmd(cmd);

			static char **ft_parse_cmd(char *cmd)
			{
				char **cmd_list;

				cmd_list = ft_split_quotes(cmd, ' ');
				if (!cmd_list || !cmd_list[0])
				{
					cmd_not_found_msg(cmd);
					ft_free_str(cmd_list);
					exit(127);
				}
				return (cmd_list);
			}

			A função ft_parse_cmd() pega numa string do comando  (ex: "ls -l /tmp") e transforma-a num array de strings (argv, que será o nosso cmd_path) 
			adequado para passarmos ao execve, respeitando as aspas.
			Caso não consiga extrair um executável válido, escreve a mensagem de comando não encontrado e sai com o exit(127).

			O pipex recebe cada comando como uma string única (vinda da linha de comandos - argv[i]), não como um array já separado.
			Logo, precisamos de transformar essa string num array de strings (transformar em argv) e respeitar as aspas para não partir
			argumentos com espaços lá dentro (ex: 'hello world'), adequado para o execve.

			O execve exige: - Um argv[0], que é o nome do executável (ex.: "grep").
						    - Um argv[1..n], que são os argumentos já separados respeitando as aspas (ex: "'hello world'", "-n").
						    - Um argv[last] = NULL (obrigatório).

			> O cmd é a string do comando tal como foi passada na linha de comandos (argv[i]).
			  Ex.: cmd = "ls -l /tmp"
			       cmd = "grep 'hello world' -n file.txt"
				   
			> O cmd_list é o array de strings (argv) que vamos passar ao execve.
			  Ex.: cmd_list = {"ls", "-l", "/tmp", NULL}
			       cmd_list = {"grep", "hello world", "-n", "file.txt", NULL}
				   
			O execve não aceita a linha única "ls - l /tmp".
			Ele precisa de um array cmd_list = {"ls", "-l", "/tmp", NULL}.

			> Para isso, usamos a função ft_split_quotes(cmd, ' '), que é uma variação do split que separa por espaços e respeita as aspas simples e as duplas.

			  Exemplo: Entrada: cmd = "grep 'hello world' -n"

			  Saída: cmd_list[0] = "grep"
					 cmd_list[1] = "hello world"   // sem as aspas
					 cmd_list[2] = "-n"
					 cmd_list[3] = "file.txt"
					 cmd_list[4] = NULL

			  Ficando cmd_list = {"grep", "hello world", "-n", "file.txt", NULL};


			  Visualmente:

			  cmd (string crua)  ──ft_split_quotes──►  cmd_list (argv)
			  "ls -l /tmp"                            {"ls", "-l", "/tmp", NULL}

			  Assim, se o cmd = "ls -l /tmp",
			  O cmd_list = {"ls", "-l", "/tmp", NULL};

			  No fim, devolvemos o cmd_list porque é exatamente o que passamos ao execve.

			  Porquê que usamos o split_quotes e não um simples split ?

			  Um split simples por espaços dividia os argumentos com espaços internos. 
			
			  Ex.: cmd = "grep 'hello world' -n file.txt"

				   Split simples → {"grep","'hello","world'","-n","file.txt"} ❌

				   Split_quotes → {"grep","hello world","-n","file.txt"} ✅

			  Assim, o execve exige este formato e o array termina em NULL.

			Vejamos como funciona o ft_split_quotes:

			char	**ft_split_quotes(char const *s, char c)
			{
				char	**strings;

				strings = (char **)malloc((count_words(s, c) + 1) * sizeof(char *));
				if (!strings)
					return (NULL);
				if (!fill_word_array(strings, s, c))
					return (NULL);
				return (strings);
			}

			static void	quote_check(char c, char *quote)
			{
				if (c == '\'' || c == '"')
				{
					if (*quote == c)
						*quote = 0;
					else if (*quote == 0)
						*quote = c;
				}
			}

			Aqui, na função quote_check(), o quote guarda o tipo de aspa que está aberta agora: - 0 = está fora da aspa;
			                                                                                    - '\'' = denota que estamos dentro de uma aspa 
																								         simples;
																								- '\"' = denota que estamos dentro de uma aspa dupla;

			Assim, quando encontramos uma aspa, verificamos se: - Se já estamos dentro dessa aspa (*quote == c), então a aspa atual é igual à que está
			                                                      aberta (se sim, fechamos a aspa, quote = 0).
			                                                    - Caso contrário, se estivermos fora das aspas (*quote == 0), então abrimos a aspa (quote = c).

			Exemplo: s = "grep 'hello world' -n file.txt".

			No início, quote = 0 (fora de aspas).
			Quando encontramos a primeira aspa simples, quote passa de 0 para quote = '\'' (está dentro de aspas simples).
			Quando encontramos a segunda aspa simples ('\'') que fecha o 'hello world', o quote volta para quote = 0 (está fora de aspas).

			static int	count_words(const char *s, char sep)
			{
				int		i;
				int		count;
				int		in_word;
				char	quote;

				i = 0;
				count = 0;
				in_word = 0;
				quote = 0;
				while (s[i] != '\0')
				{
					quote_check(s[i], &quote);
					if ((s[i] != sep || quote) && in_word == 0)
					{
						count++;
						in_word = 1;
					}
					if (s[i] == sep && !quote)
						in_word = 0;
					i++;
				}
				return (count);
			}

			Aqui, a função count_words() conta quantas palavras (tokens) existem na string s, separadas pelo separador sep (espaço ' '),
			respeitando as aspas simples e duplas.
			
			Usamos a variável quote para indicar se estamos dentro de aspas ou não.
			Usamos a variável in_word para indicar se já estamos no meio de um token (ou seja, se já estamos dentro de uma palavra ou não).
			Usamos a variável count para contar quantas palavras existem na string s.

			Para cada caractere (s[i]) da string s:
			
				1) Chamamos a função quote_check() para atualizar o estado das aspas (se estamos dentro ou fora de aspas).
				2) Se o caractere atual não for o separador (s[i] != sep) ou se estivermos dentro de aspas (quote != 0) e ainda não estivermos 
				   dentro de uma palavra (in_word == 0), então encontramos o início de uma nova palavra (o inicio de um token), pelo o que:
				   
					- Contamos como uma palavra (token) -> incrementamos o contador de palavras (count++).
					- Marcamos que agora estamos dentro de uma palavra (in_word = 1).
					
				3) Se o caractere atual for o separador (s[i] == sep) e não estivermos dentro de aspas (quote == 0), então marcamos que saímos
				   da palavra (in_word = 0).
				4) Continuamos até ao fim da string.

			Exemplo: s = "grep 'hello world' -n".

			No início, quote = 0 (está fora de aspas), in_word = 0 (está fora da palavra) e count = 0.

			AO percorrer a string, contamos o grep como uma palavra (token), pois é um conjunto de caracteres que não é o separador (espaço) e
			ainda  não estavamos dentro de uma palavra (in_word == 0). Assim, count = 1 e in_word = 1 (estamos dentro da palavra grep).
			De seguida, encontramos um espaço (separador) e não estamos dentro de aspas (quote == 0), pelo o que marcamos que saímos da palavra (in_word = 0).
			Quando encontramos a aspa simples ('\'') de 'hello world', chamamos a função quote_check() que atualiza o estado das aspas (quote passa de 0
			para quote = '\'') e indicamos que estamos dentro de aspas (quote != 0), pelo o que 'hello world' é contado como uma palavra (token) inteira, 
			mesmo tendo um espaço lá dentro, pois estamos dentro de aspas (quote != 0). Assim, count = 2 e in_word = 1 (estamos dentro da palavra 'hello world').
			A seguir, encontramos outro espaço (separador) e não estamos dentro de aspas (quote == 0), pelo o que marcamos que saímos da palavra (in_word = 0).
			Finalmente, encontramos o -n, que é contado como uma palavra (token), pois não é o separador (espaço) e ainda não estamos dentro de uma palavra (in_word == 0).
			Assim, count = 3 e in_word = 1 (estamos dentro da palavra -n).
			No fim, count = 3 (três tokens: "grep", "hello world", "-n").


			static void	free_all(char **arr, int size)
			{
				while (size >= 0)
				{
					free(arr[size]);
					size--;
				}
				free(arr);
			}

			static int	fill_word_array(char **arr, char const *s, char c)
			{
				int		i;
				int		len;
				int		start;
				char	delimiter;

				i = 0;
				start = 0;
				while (i < count_words(s, c))
				{
					delimiter = c;
					while (s[start] == c)
						start++;
					if (s[start] == '\'' || s[start] == '\"')
						delimiter = s[start++];
					len = 0;
					while (s[start + len] && s[start + len] != delimiter)
						len++;
					arr[i] = ft_substr(s, start, len);
					if (!arr[i])
						return (free_all(arr, i - 1), 0);
					start += len + 1;
					i++;
				}
				arr[i] = NULL;
				return (1);
			}

			A função fill_word_array() preenche o array arr com as palavras (tokens) extraídas da string s, separadas pelo separador c (espaço ' '), 
			e lida com aspas simples e duplas.

			> while (i < count_words(s, c))
			
			  Percorre cada token (palavra) que contamos com a função count_words().
			  O array arr tem espaço para todos os tokens, pois foi alocado com malloc na função ft_split_quotes() com o tamanho
			  correto (count_words(s, c) + 1). Assim, este loop vai produzir exatamente o nº de tokens contados.

    		> delimiter = c;

        	  Assumimos que o delimitador de fim da palavra (token) é o separador normal c (por exemplo, o espaço).
			  
	
    		> while (s[start] == c) 
			  	start++;

        	  Aqui vamos avançar o índice start até encontrar o início da próxima palavra (token), ou seja, avançamos o start até encontrar um 
			  caractere que não seja o separador c.

    		> if (s[start] == '\'' || s[start] == '\"') 
				delimiter = s[start++];

        	  Verificamos se o primeiro caractere do token é uma aspa simples ou uma aspa dupla.
			  Se for uma aspa, o delimitador do token deixa de ser o espaço e passa a ser a própria aspa, pois queremos parar apenas no 
			  fecho das aspas.
			  Avançamos o índice start para o próximo caractere (avançamos para depois da aspa inicial), pois, assim, não copiamos a aspa de abertura
			  do token e a aspa inicial não faz parte do conteúdo do token (mais tarde também não copiamos a aspa do fecho, porque o loop pára antes dela).
			  
    		> len = 0; 
			  while (s[start + len] && s[start + len] != delimiter) 
			  	len++;

			  Aqui vamos contar o tamanho do token (len) até encontrar o delimitador (delimiter), ou seja, vamos contar quantos caracteres existem no token
			  (entre o início do token - start - e o delimitador).
			  
			  Se delimiter == c (sem aspas iniciais), então o token vai até ao próximo separador c (espaço) ou até ao fim da string.
			  Assim, se não começou com aspa, o delimiter é o separador (espaço).
			  
			  Se o delimiter for uma aspa (com aspas iniciais), então o token vai até à próxima aspa igual (até à aspa de fecho) ou até ao fim da string.
              Assim, se começou com aspa, o delimiter é '\'' ou '\"'.
			  
              Isto faz com que as aspas externas sejam removidas: não entram na substring.
			  Notamos que o loop pára antes de contar o delimitador, ou seja, o delimitador não faz parte do token.

			  Exemplos rápidos:

				s = "ls -l /tmp", c = ' '
				Tokens: "ls", "-l", "/tmp".

				s = "grep 'hello world' -n", c = ' '
				Tokens: "grep", "hello world", "-n".
				(As aspas foram removidas do segundo token.)

    		> arr[i] = ft_substr(s, start, len);

			  A função ft_substr() aloca uma nova string (com malloc) que é uma substring da string s, com os len caracteres a partir do índice s + start.
			  Assim, cria uma nova string que contém o token extraído da string s, começando no índice start e com comprimento len.
			  Esta nova string é armazenada no array arr na posição i (arr[i]).
			  Assim, arr[i] contém o token extraído da string s.

              Ex.: para 'hello world', copia apenas hello world.

    		> if (!arr[i]) 
			  {
				free_all(arr, i - 1);
				return (0);
			  }

              Se falhar a alocação de memória (arr[i] == NULL), chamamos a função free_all() para libertar todas as strings já alocadas no array arr

    		> start += len + 1;

			  len caracteres: avançamos o conteúdo que copiámos.
              + 1: saltamos o delimitador que parou o loop: - Se o delimiter == c, saltamos o separador (ex.: espaço).
                                                            - Se o delimiter é '\'' ou '\"', saltamos a aspa de fecho.

			  É isto que faz com que as aspas não aparecerem nos tokens finais: a de abertura foi “consumida” quando fizemos start++ lá atrás; 
			  A de fecho é evitada pelo while (... != delimiter) e depois ignorada com o + 1.

              Isto posiciona start no início para o próximo token (ou no '\0').

			> i++;

			  Avançamos para o próximo índice do array arr para armazenar o próximo token.
			  Assim, sucessivamente, arr[0], arr[1], arr[2], ... vão sendo preenchidos com os tokens extraídos da string s.

            > arr[i] = NULL; 
			
			  No fim, fechamos o array com NULL para terminar o array ao estilo argv e o que o execve exige.

    
			  

			1) if (!cmd_list || !cmd_list[0])

				> !cmd_list → Verifica o caso de erro no malloc (falha ao criar o array de strings), em que o ft_split_quotes devolve NULL,
				              pelo o que não há um executável (argv) para passarmos ao execve.

				> !cmd_list[0] → Verifica se o primeiro token não existe, ou seja, se não há qualquer token (o argv está vazio). 
				                 O array existe, mas o primeiro elemento é NULL, pelo o que não há um executável (argv[0]) para passarmos ao execve.
								 Como vimos, para o execve isto é inválido, pois o execve exige que o nome do programa (cmd_list[0]) exista.
								 Sem o cmd_list[0] não há comando para executar no programa. Assim tratamos isto como um "comando não encontrado".
								 
								 Ex: cmd = ""  ou cmd = "    "  (só espaços)
								     O ft_split_quotes devolve um array com o primeiro elemento a NULL -> em termos de argv: {"\0" …} na prática é { NULL }.


                Se qualquer uma destas condições for verdadeira, escrevemos a mensagem de comando não encontrado, libertamos todas as strings do cmd_list
				e saímos com o exit(127) (código de erro de comando não encontrado).
				
				EM caso de sucesso, devolvemos o cmd_list.
				

			2)  cmd_path = handle_cmd_path(cmd, cmd_list, envp);

				static char *handle_cmd_path(char *cmd, char **cmd_list, char **envp)
				{
					char *cmd_path;

					if (ft_strchr(cmd_list[0], '/'))
					{
						check_cmd_access(cmd, cmd_list);
						cmd_path = ft_strdup(cmd_list[0]);
					}
					else
					{
						cmd_path = ft_cmd_path(cmd_list[0], envp);
						if (!cmd_path)
						{
							cmd_not_found_msg(cmd_list[0]);
							ft_free_str(cmd_list);
							exit(127);
						}
					}
					return (cmd_path);
				}

				Na função handle_cmd_path(), o objetivo é determinar o caminho completo do executável a partir do nome do comando (cmd_list[0])
				e do ambiente (envp), para podermos passar esse caminho ao execve.

				> O cmd é a string original do comando tal como foi passada na linha de comandos (argv[i]).
				  Ex.: cmd = "ls -l /tmp"
				       cmd = "grep 'hello world' -n file.txt"

				> O cmd_list é o array de strings (argv), obtido pela função ft_split_quotes,que vamos passar ao execve.
				  Ex.: cmd_list = {"ls", "-l", "/tmp", NULL}
				       cmd_list = {"grep", "hello world", "-n", "file.txt", NULL}

				> O cmd_path é o caminho completo do executável que vamos passar ao execve.
				  Ex.: cmd_path = "/bin/ls"
				       cmd_path = "/usr/bin/grep"

				  Como vamos ver, o caminho completo do executável pode ser:
				  
				    - Um caminho absoluto (começa com '/') que indica o local exato do executável no sistema (indica a raiz do sistema de ficheiros),
					  por exemplo: "/bin/ls".
					  O execve usa exatamente este caminho absoluto.
				    - Um caminho relativo (contém '/', mas não começa com '/') que indica o local do executável em relação à pasta atual,
					  por exemplo: "./myprog" ou "subdir/tool".
					  O execve usa exatamente este caminho relativo.
				    - Um nome simples (não contém '/'), por exemplo: "ls" ou "grep".
					  Neste caso, não pode ser apenas o nome, pois o execve não procura no PATH quando o caminho nao contém o '/'. 
					  Se assim for o caso de ser apenas o nome, precisamos de transformar este nome num caminho completo do executável, procurando 
					  no PATH e construindo o caminho completo com a função a ft_cmd_path().

				Portanto nesta função handle_cmd_path(), temos dois casos principais a tratar:

				  - Se o cmd_list[0] tiver '/', indica que já é um caminho (absoluto ou relativo).
				  	Neste caso, não procuramos no PATH, apenas validamos se o caminho existe e se é executável - função check_cmd_access().
					Se passar na validação, usamos tal-e-qual como está (cmd_path = strdup(cmd_list[0])).

				  - Se o cmd_list[0] não tiver '/', indica que é apenas um nome simples.
				  	Neste caso, temos de procurar no PATH e construir um caminho válido para passarmos ao execve - função ft_cmd_path().
					
				Se não encontrarmos um caminho válido, retornamos NULL, escrevemos a mensagem de comando não encontrado e saímos com o exit(127).

				No fluxo:  - Se cmd_list[0] contém '/': - Não procuramos no PATH.
														- Validamos aquele caminho específico com o check_cmd_access.
														- Se passar, fazemos cmd_path = ft_strdup(cmd_list[0]) e seguimos para o execve(cmd_path, cmd_list, envp).

							- Se não contém '/': - Não chamamos o check_cmd_access. Em vez disso, percorremos o PATH (função ft_cmd_path) à procura de um executável 
							                       com esse nome.

				Por que não procurar no PATH quando o cmd_list[0] é '/'?

					O comando com '/' é interpretado como o caminho fornecido pelo utilizador. 
					O execve nunca consulta o PATH. Ele tenta abrir exatamente o que passamos. 
					Os shells só procuram no PATH quando não há '/' no argv[0].     
					

				1) Caso tenha '/' no cmd_list[0]:

				  if (ft_strchr(cmd_list[0], '/'))

				  A função ft_strchr() verifica se a string cmd_list[0] contém o caractere '/'.
				  Se sim, interpretamos que o utilizador especificou um caminho (absoluto ou relativo).
				
				  Por exemplo: cmd_list[0] = "/bin/ls" -> absoluto
				   	           cmd_list[0] = "./myprog" -> relativo
					           cmd_list[0] = "subdir/tool" -> relativo

				  Se houver '/', não procuramos no PATH, apenas validamos o caminho fornecido com a função check_cmd_access().

				  if (ft_strchr(cmd_list[0], '/'))
				  {
					check_cmd_access(cmd, cmd_list);
					cmd_path = ft_strdup(cmd_list[0]);
				  }

				  Aqui, significa que o utilizador forneceu um caminho explicito para o executável, não apenas um nome.
				  Se começa por '/', o caminho do executável é absoluto. Por exemplo "/bin/ls".
				  Se contém o '/' mas não começa com '/' o caminho do execútavel é relativo. Por exemplo "./prog".
				
				  De seguida, chamamos a função check_cmd_access():

				  static void check_cmd_access(char *cmd, char **args)
				  {
					if (access(args[0], F_OK) != 0)
					{
						path_not_found_msg(cmd);
						ft_free_str(args);
						exit(127);
					}
					else if (access(args[0], X_OK) != 0)
					{
						perror("Pipex");
						ft_free_str(args);
						exit(126);
					}
				  }

				  > if (access(args[0], F_OK) != 0)

				    Aqui verificamos se o caminho especificado em args[0] (cmd_list[0]) existe.
					A função access() verifica a existência do ficheiro (F_OK).
					
					Caso o ficheiro não exista, retornamos -1, escrevemos a mensagem de command not found, libertamos o cmd_list alocado e
					saímos com o exit(127) (código de erro de comando não encontrado).

				  > else if (access(args[0], X_OK) != 0)

				    Só entramos neste else if caso o caminho exista.
					Aqui verificamos se temos permissão para executar o ficheiro especificado em args[0] (cmd_list[0]).
					A função access() verifica a permissão de execução do ficheiro (X_OK).

					Caso não tenha, retorna -1, a função escreve a mensagem permission denied, liberta o cmd_list e sai com o 
					exit (126).

				  > cmd_path = ft_strdup(cmd_list[0]);

				    Assim, se o caminho existir e for executável, duplicamos a string cmd_list[0] para o cmd_path com o ft_strdup(), 
				    para termos o caminho do executável alocado, que vamos passar ao execve.

					
				2) Caso não tenha '/' no cmd_list[0]

				   cmd_path = ft_cmd_path(cmd_list[0], envp);
				   if (!cmd_path)
				   {
					cmd_not_found_msg(cmd_list[0]);
					ft_free_str(cmd_list);
					exit(127);
				   }

				   Se o cmd_list[0] não contém o '/', temos de percorrer o PATH (fazemos isso com a função ft_cmd_path) à procura de um 
				   executável com esse nome e construir um caminho executável para passarmos ao execve.

				   Aqui, o cmd_list[0] é apenas um nome simples, por exemplo: "ls" ou "grep".
				   A função ft_cmd_path() vai procurar esse nome nos diretórios do PATH e devolve o primeiro caminho completo que encontrar.

				   Porquê que procuramos pelo PATH ?

				   Porque o execve não procura no PATH. Ele tenta abrir exatamente o que passamos.
				   Os shells só procuram no PATH quando não há '/' no argv[0].
				   Assim, se o cmd_list[0] não tem '/', temos de procurar no PATH para encontrar o executável.
				   
				   O objetivo é transformar um nome do comando/executável (cmd) num caminho executável (cmd_path) para passarmos ao execve.
				   Caso não haja um caminho válido, retornamos NULL, escrevemos a mensagem de command not found , libertamos o cmd_list e 
				   saimos com o exit(127).
										
					char *ft_cmd_path(char *cmd, char** envp)
					{
						char **paths;
						char *fullpath;
						int i;

						if (!cmd || !envp)
							return (NULL);
						paths = ft_get_env_paths(envp);
						if (!paths)
							return (NULL);
						i = 0;
						while (paths[i])
						{
							fullpath = ft_join_dir_cmd(paths[i], cmd);
							if (fullpath && access(fullpath, F_OK | X_OK) == 0)
								return (ft_free_str(paths), fullpath);
							free(fullpath);
							i++;
						}
						ft_free_str(paths);
						return (NULL);
					}

					A função ft_cmd_path() tem como objetivo transformar o nome do comando - cmd (por exemplo: "ls") - num caminho absoluto - cmd_path (por
					exemplo: "/usr/bin/ls") - que pode ser usado para executar o comando. 
					Ela faz isso ao procurar pelo comando nos diretórios listados na variável de ambiente PATH e devolvendo o primeiro caminho válido encontrado.

					> O que é o envp ?

					  O envp é um array de strings que contém as variáveis de ambiente do processo.
					  Cada string tem o formato "NOME=valor" e representa uma variável de ambiente visivel para o processo no momento em que ele foi criado.
					  Ele contém informações importantes sobre o ambiente em que o programa está a ser executado, como o PATH, HOME, USER, etc.
					  
					  Exemplo: envp = {"PATH=/usr/local/bin:/usr/bin:/bin",
										"HOME=/home/user","LANG=pt_PT.UTF-8",
										"SHELL=/bin/bash","PWD=/home/user/projeto", NULL}
					  
					  As variáveis de ambiente são pares nome-valor que armazenam informações sobre o ambiente do sistema e do utilizador.

					  Exemplos comuns de variáveis de ambiente:
					  
					  PATH — lista de diretórios onde procurar os executáveis quando escrevemos apenas o nome (ls, grep, …).
					  HOME — diretório do utilizador (usado por programas para achar ficheiros de configuração, etc.).
					  PWD — diretório corrente (informativo).


					Para isso, a função ft_cmd_path(), através da função ft_get_env_paths(), vai ler o PATH da variável de ambiente e dividir o PATH em diretórios individuais,
					convertendo-o num array de strings (numa lista de diretóŕios) - paths.
					Por exemplo, se o PATH for "/usr/local/bin:/usr/bin:/bin", o array paths será { "/usr/local/bin", "/usr/bin", "/bin", NULL }.
					
					Depois, para cada diretório dir dessa lista, é construído um caminho candidato (diretório + '/' + cmd) que será verificado se ele existe e se é executável.
					Ao encontrar o o primeiro caminho válido, esse caminho é devolvido. 
					Se nenhum passar, devolve NULL.
					
					Isto replica o que o shell faz quando escrevemos um comando sem '/' (ex: "ls") - o shell percorre os diretórios do PATH à procura do executável.

					> if (!cmd || !envp)
						return (NULL);

					  Verifica se o cmd ou o envp são NULL.
					  Sem o comando ou sem o ambiente, não há como procurar o comando, pelo o que retornamos NULL.
					
					> paths = ft_get_env_paths(envp);

					  A função ft_get_env_paths vai procurar a string PATH= no envp, extrai a parte depois de PATH=, e faz split(':').
					  Se o PATH não existir (ex.: env -i), devolvemos NULL. Quem chama deve tratar isso como “comando não encontrado” (127) a não ser que cmd já viesse com '/' 
					  (nesse caso, normalmente tratar-se-ia noutro ramo do código).

					> static char **ft_get_env_paths(char **envp)
					{
						char **paths;
						char *env_path;
						int i;

						if (!envp)
							return (NULL);
						i = 0;
						paths = NULL;
						env_path = NULL;
						while (envp[i])
						{
							if (ft_strncmp(envp[i], "PATH=", 5) == 0)
							{
								env_path = envp[i] + 5;
								break;
							}
							i++;
						}
						if (!env_path)
							return (NULL);
						paths = ft_split(env_path, ':');
						if (!paths)
							exit(1);
						return (paths);
					}

					Aqui, a partir do envp (o array de variáveis de ambiente no formato NOME=valor) vamos procurar a variável de ambiente PATH, extrair apenas o
					o seu valor (o conteúdo após a string "PATH=", ou seja, a lista de diretórios separada por ':') e dividi-la num array de strings, onde cada 
					elemento é um diretório individual. 
					Isto é necessário para obter um array de diretórios onde vamos procurar os executáveis.
					
					No final, é retornado um array de strings em que cada elemento é um diretório vindo da variável de ambiente
					PATH, na ordem em que aparecem.

					Exemplo do envp:

					envp[0] = "SHELL=/bin/bash"
					envp[1] = "USER=rui"
					envp[2] = "PATH=/usr/local/bin:/usr/bin:/bin:/home/rui/bin"
					envp[3] = "HOME=/home/rui"
					envp[4] = NULL

					Encontra a variável de ambiente PATH no indice 2, e extraimos o valor "/usr/local/bin:/usr/bin:/bin:/home/rui/bin".
					
					De seguida, fazemos o split por ':' para obter o array: paths[0] = "/usr/local/bin"
																			paths[1] = "/usr/bin"
																			paths[2] = "/bin"
																			paths[3] = "/home/rui/bin"
																			paths[4] = NULL
																								

					> Se o ambiente for nulo, não há nada para procurar no PATH.

						if (!envp)
							return (NULL);

					> Procurar a variável de ambiente PATH=

						while (envp[i])
						{
							if (ft_strncmp(envp[i], "PATH=", 5) == 0)
							{
								env_path = envp[i] + 5;
								break;
							}
							i++;
						}

						Percorremos cada variável de ambiente envp[i].
						Quando encontramos uma que começa por "PATH=", fazemos env_path = envp[i] + 5, para avançarmos
						5 caracteres, saltando o "PATH=", e ficando a apontar só para o valor.

						Ex.: se envp[i] == "PATH=/usr/bin:/bin", então env_path == "/usr/bin:/bin".

						Se não encontrar, env_path continua NULL.

					> Se não encontrar o PATH= retorna NULL

					if (!env_path) 
						return (NULL);

					Sem o PATH, não há diretórios padrão para procurar os executáveis. 
					O chamador tratará como “command not found” (127), tal como um shell faria com PATH vazio/ausente.

					> Dividir por ':'

					paths = ft_split(env_path, ':');
					if (!paths)
						exit(1);

					O PATH é uma string com diretórios separados por ':'.

					Ex: "PATH=/usr/local/bin:/usr/bin:/bin".

					QUando alguém invoca um comando sem '/' (por exemplo, o ls), procuramos o binário nesta ordem:

						"/usr/local/bin", depois "/usr/bin" e depois em "/bin".

					Para conseguirmos iterar por estes diretórios, precisamos de convertê-los num array de strings.
					É isso que a função ft_split() faz:

					env_path: "/usr/local/bin:/usr/bin:/bin"
											│
							ft_split(':') ───┴──► paths = { "/usr/local/bin", "/usr/bin","/bin", NULL }

					No final, é retornado um array de strings em que cada elemento é um diretório vindo da variável de ambiente
					PATH, na ordem em que aparecem.

					Se a alocação falhar, saimos com exit(1).

					Esquema:

					envp ──► ["PATH=/usr/local/bin:/usr/bin:/bin", "HOME=/home/u", ... , NULL]

					Após localizarmos o PATH: env_path ──► "/usr/local/bin:/usr/bin:/bin"

					Após o split: paths ──► { "/usr/local/bin", "/usr/bin", "/bin", NULL }


					> Percorremos os diretórios da variável de ambiente PATH:

					while (paths[i])
					{
						fullpath = ft_join_dir_cmd(paths[i], cmd);
						if (fullpath && access(fullpath, F_OK | X_OK) == 0)
							return (ft_free_str(paths), fullpath);
						free(fullpath);
						i++;
					}

					O objetivo deste while é percorrer cada diretório do PATH, construir um candidato [dir + '/' + cmd],
					testar se é executável e devolver o primeiro que for válido.

					O array **paths é um array de strings que contém os diretórios do PATH, e contém, 
					por exemplo: 

					paths ──► { "/usr/local/bin", "/usr/bin", "/bin", NULL }

					Nós vamos começar por percorrer este array de strings (while (paths[i])).

					De seguida, começamos por construir o candidato: fullpath = ft_join_dir_cmd(paths[i], cmd);

					Concatenamos o diretório com o comando: "<dir>/<cmd>".

					Por exemplo: paths[i] = "/usr/bin", e o cmd = "ls" → fullpath = "/usr/bin/ls".

					static char *ft_join_dir_cmd(char *dir, char *cmd)
					{
						char *dir_slash;
						char *fullpath;

						if (!dir || !cmd)
							return (NULL);
						dir_slash = ft_strjoin(dir, "/");
						if (!dir_slash)
							return (NULL);
						fullpath = ft_strjoin(dir_slash, cmd);
						free(dir_slash);
						if (!fullpath)
							return (NULL);
						return (fullpath);
					}

					> Se não houver diretório ou se não houver comando não há como montar o caminho.
					  Retornamos NULL.

					  if (!dir || !cmd)
						   return (NULL);

					> Primeira junção: dir + "/"

						dir_slash = ft_strjoin(dir, "/");

						Aqui alocamos uma nova string com o conteúdo do dir (paths[i]) seguido de um "/".

						Por exemplo: "/usr/bin" → "/usr/bin/"

					> Segunda junção: (dir + "/") + cmd

						fullpath = ft_strjoin(dir_slash, cmd);

						Alocamos outra nova string concatenando o cmd no fim.

						Por exemplo: "/usr/bin/" + "ls" → "/usr/bin/ls".

					> Libertamos a string temporária dir_slash, pois já nao precisamos mais dela

						free (dir_slash)

					> Verificamos a alocação final se correu tudo bem. Caso contrário retornamos NULL.

						if (!fullpath) 
							return NULL;

						
					> Verificar se o caminho construído (fullpath) pode ser executado

						if (fullpath && access(fullpath, F_OK | X_OK) == 0)
							return (ft_free_str(paths), fullpath);
						free(fullpath);

						O fullpath acabou de ser criado.
						Veio de ft_join_dir_cmd(paths[i], cmd) e contém algo como "/usr/bin/ls".
						Pode ser NULL se faltou memória ou se a função falhou.

						Verificamos com o access(fullpath, F_OK | X_OK) se: - F_OK → o ficheiro existe;
																			- X_OK → tem permissão de execução.

						Se retorna 0, passou nos testes.
						Encontramos um candidato válido → return (ft_free_str(paths), fullpath);
						Libertamos o array dos diretórios do PATH e depois devolvemos o fullpath. Assim não há leak de paths.

						Se falhou → free(fullpath) e continua o loop.
						O caminho testado não serve (não existe, não executável, etc.).
						Liberta o fullpath desta iteração para não vazar memória e vai testar o próximo paths[i+1].


					> Execução do execve
						
					if (execve(cmd_path, cmd_list, envp) == -1)
					{   
						perror("execve failed");
						ft_free_str(cmd_list);
						free(cmd_path);
						if (errno == EACCES)
							exit(126);
						else if (errno == ENOENT)
							exit(127);
						exit(EXIT_FAILURE);
					}  

					O execve tem como objetivo substituir a imagem do processo atual pelo programa que está no cmd_path.
					Torna-o num binário executável, passando-lhe os argumentos (cmd_list) e o ambiente (envp).

					O binário contém: - Mesmo PID, novo programa. O processo não “cria” outro: ele torna-se o novo programa.
					                  - Argumentos: argv (terminado em NULL, com argv[0] sendo o nome do programa).
									  - Ambiente: envp (também terminado em NULL), as variáveis de ambiente do novo programa.
					                  - FDs: continuam os mesmos (stdin=0, stdout=1, stderr=2 e quaisquer outros).
									  - Memória/código: todo o código/dados/heap/stack do processo atual é descartado; 
									                    o kernel carrega o executável e mapeia as suas secções;

					Como acontece a substituição ?

					1) Verificações: o ficheiro existe? tem permissão de executar? (senão, -1 com errno: ENOENT, EACCES, …)
					2) Tipo de ficheiro: Binário → carrega segmentos de código/dados.
					3) Carregamento: o kernel mapeia o novo programa no mesmo PID.
					4) Stack nova: coloca o argv e o envp na stack do novo programa.
					5) Mantém abertos os FDs (por isso os redirecionamentos 0/1 feitos com dup2 permanecem).
					6) Transita para o novo programa: começa a correr o novo código.

					-> O PID continua a ser o mesmo.
					-> O programa (código, dados, heap) é substituido pelo programa novo.

					O cmd_path é o caminho do executável. Ex: "/bin/ls".
					O cmd_list é o array de strings com os argumentos (cmd_list[0] é o nome do programa).
							
					Em caso de sucesso, o execve não retorna nada, porque o processo atual é substituído pelo novo programa e já é 
					esse programa que está a correr.
					O processo (com o mesmo PID) agora é o novo programa.
					O novo programa começa como se tivesse começado do zero, mas o argv e o envp são os que passamos e os FDs continuam
					abertos com as mesmas ligações que fizemos.

					O execve é o passo que transforma cada filho no comando real da pipeline. 
					Com ele, cada filho passa a ser o programa pedido, herdando o stdin/stdout já ligados ao que precisa.

					No pipex: - O child 1 agora é o cmd 1 e passa a escrever no pipe (porque o seu stdout foi redirecionado/ligado ao write end).
							  - O child 2 agora é o cmd 2 e passa a ler do pipe (porque o seu stdin foi redirecionado/ligado ao read end).


							
	> Gerir múltiplos comandos e múltiplos pipes
							
	  int main(int ac, char **av, char **envp)
	  {
		int prev_read_fd;
		int last_pid;
		int exit_code;
		int i;

		if (ac < 5)
			show_usage_exit2();
		if (ft_strncmp(av[1], "here_doc", 8) != 0)
		{
			first_child(av, &prev_read_fd, envp);
			i = 3;
			while (i < ac - 2)
			{
				last_pid = middle_child(av, &prev_read_fd, envp, i);
				i++;
			}
			last_pid = last_child(ac, av, prev_read_fd, envp);
		}
		else
			last_pid = ft_heredoc_pipeline(ac, av, envp);
		exit_code = wait_processes(last_pid);
		return (exit_code);
	  }
	  
	  
	  void	first_child(char **av, int *prev_readfd, char **envp)
	  {
		int	infile_fd;
		int pipefd[2];
		pid_t pid;
		
		get_pipe_and_fork(pipefd, &pid);
		if (pid == 0)
		{
			close(pipefd[0]);
			infile_fd = open(av[1], O_RDONLY);
			if (infile_fd == -1)
			{
				close(pipefd[1]);
				error_exit("Error Opening Input File!");
			}
			safe_dup2(infile_fd, STDIN_FILENO);
			safe_dup2(pipefd[1], STDOUT_FILENO);
			close(pipefd[1]);
			close(infile_fd);
			ft_exec_cmd(av[2], envp);
		}
		close(pipefd[1]);
		*prev_readfd = pipefd[0];
	  }
	  
	  a1) Primeiro chamamos a função get_pipe_and_fork() para criar o pipe e o criar o primeiro processo (o primeiro comando).

	      get_pipe_and_fork(int pipefd[2], pid_t *pid)

		  O kernel cria um objeto pipe com um buffer e devolve dois FDs abertos que apontam para as duas pontas do mesmo pipe:
	      
            - pipefd[0]: read end -> aberto só para leitura do pipe: associado ao FD de leitura;
            - pipefd[1]: write end -> aberto só para escrita no pipe: associado ao FD de escrita;

		  Tabela de FDs (no parent) logo após pipe:

		  FD 0 (STDIN)  -> aponta para o terminal/ficheiro
		  FD 1 (STDOUT) -> aponta para o terminal/ficheiro
	      FD 2 -> stderr
	      FD 3 -> aponta para o pipefd[0]  (read end)
	      FD 4 -> aponta para o pipefd[1]  (write end)
            
          O fork() cria um novo processo (child 1) que é uma cópia do parent.
		  O child herda a mesma tabela de FDs do parent, ou seja, o child 1 também tem o pipefd[0] e o pipefd[1] abertos.

		  Depois do fork(), tanto o child como o parent têm as duas pontas do pipe abertas:

	  	  Parent (após o fork):  FD 3 -> pipefd[0]
						         FD 4 -> pipefd[1]

		  Child 1 (cópia):  FD 3 -> pipefd[0]
					        FD 4 -> pipefd[1]
		  
		  O fork() devolve o PID do child no parent (pid > 0) e devolve 0 no pid do child (pid == 0).

		  Por que criar o pipe antes do fork?
		  
		  Para que tanto o parent quanto o child possuam os FDs do pipe e cada um feche o que não usa. 
		  Isso é essencial para a correta sinalização do EOF e para evitar deadlocks.

	 a2) Se pid == 0, entramos no child 1 (que vai ser o nosso cmd1).
	     Começamos por fechar o read end do pipe (pipefd[0]) neste processo, pois este processo não vai ler do read end do pipe, só 
		 vai escrever no write end do pipe (o processo que vai ler (o leitor) vai ser o comando seguinte a este).

		 O kernel mantém os contadores: o número de pontas de leitura que estão abertas (pipefd[0]) e o número de pontas de
		 escrita que estão abertas (pipefd[1]) do pipe.
		 Se o processo que vai escrever no pipe (o escritor, que neste caso é o child 1) mantiver aberta a read end (pipefd[0]), então o 
		 número de referencias ao read end será maior do que 0 (> 0) - ou seja, o número de pontas de leitura será > 0 - mesmo que o 
		 verdadeiro processo que vai ler do pipe (o leitor, que é o child 2) já tenha fechado/morrido.
		 Assim, quando o processo leitor real fechar, o processo escritor continua a escrever até encher o buffer, e então bloqueia
		 para sempre, pois ninguém está a consumir (ler).

		-> Fechar o read end (pipefd[0]) no child 1 faz com que o número de pontas de leitura abertas (pipefd[0]) seja a correta,
		   assim, quando o child 2 morrer, esse número será 0 e write no child 1 já não bloqueia.
		 
		-> Ao fechar o read end, garantimos que o contador de referências do read end do pipe está correto.
		
	
	 a3) infile = open(av[1], O_RDONLY);

	     Aqui, vamos abrir o ficheiro infile (av[1]) para lermos a partir dele o conteúdo que será a entrada do child 1.

	     A função open() abre um ficheiro e devolve um FD. 
		 O kernel vai percorrer o ficheiro (av[1]) e verifica, com o O_RDONLY, se esse ficheiro tem permissão só de leitura. 
	     Se tudo estiver OK, é devolvido um FD novo (um número inteiro >= 0) que será armazenado no infile_fd.

	     Se o open() falhar ao abrir o ficheiro infile (-1) - ou por um ficheiro inexistente, permissão negada, etc - o child 1 não irá 
		 conseguir ligar/redirecionar o STDIN para o FD do infile e não vai conseguir executar o child 1 com sucesso.
		 Portanto, fechamos o write end do pipe (pipefd[1]) no child, pois se não fechassemos o pipefd[1] aqui, o processo leitor (o child 2)
		 poderia ficar bloqueado à espera de ler mais dados do read end do pipe ou do EOF (pois haveria ainda um escritor aberto, o pipefd[1] do child 1), mesmo que 
		 o parent e o child 1 já tivessem morrido).
		 Saímos com o error_exit() para terminar o processo com o código de erro apropriado.

	     Porquê fechar aqui? 

		 Como vimos, o objeto pipe no kernel tem contadores: - o número de referencias para o read end (quantas pontas de
															   leitura estão abertas em todos os processos).
															 - o número de referencias para o write end (quantas pontas de 
															   escrita estão abertas em todos os processos).

		Se o child 1 morrer/terminar sem fechar, continuaria a existir (durante algum tempo) um pipefd[1] vivo, assim, o processo
		leitor (child 2) pode não ver o EOF quando devia e ficaria bloqueado à espera de mais dados.


	 a4) safe_dup2(int oldfd, int newfd)

	     A função dup2(int oldfd, int newfd), tem como objetivo fazer com o que o newfd passe a apontar exatamente para o mesmo 
	     objeto do que o oldfd.
	     Ou seja, o FD do newfd passa a apontar para o mesmo alvo (o mesmo ficheiro/pipe,...) que o FD do oldfd.
	     Em caso de sucesso, devolve o newfd.

		 O dup2() não copia bytes, ele duplica a ligação do oldfd, para o mesmo objeto aberto do kernel, utilizando um FD diferente,
		 o newfd.
		 
		 O dup2(), no caso de o newfd já estiver aberto, ele fecha-o antes de fazer a ligação. Assim, se o oldfd != newfd e newfd estiver
		 aberto, o kernel fecha o newfd (dentro da mesma syscall).
	     De seguida, redireciona/aponta o newfd para o mesmo objeto que o oldfd aponta.
	     Assim, newfd e oldfd passam a apontar para o mesmo objeto.

	     O dup2 é assim uma ferramenta para redireções (ligar stdin/stdout a pipes/ficheiros).

		 Cada ponta do pipe tem uma função diferente: -> write end (pipefd[1]): ponta do pipe onde escrevemos.
													  -> read end (pipefd[0]): ponta do pipe onde se lê.

		 Com estes dois FDs, ligamos o output (o STDOUT) de um processo ao input (ao STDIN) de outro ao usar a função dup2() e fechamos os 
		 FDs que não usamos.

		 dup2(int FD_origem, int FD_destino): 1) Se o FD_destino estiver aberto, o kernel fecha-o (exceto quando FD_origem == FD_destino, não faz nada, e não fecha).
											  2) Depois, faz o FD_destino apontar para o mesmo objeto que o FD_origem aponta.
												 Ficamos com duas referencias para o mesmo objeto.
											  3) Não copia bytes, só copia a ligação.
											  4) Devolve o FD_destino.

		  Consequência: POr exemplo, após dup2(infile_fd, STDIN_FILENO), tanto o STDIN (FD 0) como o FD do infile apontam para o mesmo infile_fd (há assim duas referencias).
						Por isso, teremos de fechar o infile_fd e ficamos só com o FD 0 como referência.

		  Genericamente, depois do dup2(): - dup2(pipefd[1], STDOUT) → o FD 1 (stdout) passa a escrever no write end (pipefd[1]) do pipe.
					                       - dup2(p[0], STDIN) → o FD 0 (stdin) passa a ler do read end (pipefd[0]) do pipe.

		  A função dup2() não cria um stdin/stdout novos, ela apenas aponta os FDs 0 ou 1 para a mesma ponta do pipe (read ou write end) ou para o mesmo ficheiro, aumentando
		  temporariamente a referencia até fecharmos os FDs antigos.

		  Porquê usar a função dup2() ?

		  Os programas não sabem que existe um pipe: eles escrevem na sua saída padrão (FD 1) e lêem da entrada padrão (FD 0).
		  Para qualquer comando funcionar normalmente, ele deve continuar a utilizar o seu stdin/stdout.
		  A função dup2() é a forma correta de redirecionar os FDs padrão (0, 1, 2) para outros FDs.
		  O dup2(oldfd, newfd) faz com que o newfd passe a apontar para o mesmo objeto que o oldfd aponta.
		  Assim, redirecionamos o stdin/stdout para onde queremos.
				
		  Porquê fechar o read end/write end (p[0]/p[1]) depois do dup2 ?

		  > Garantir o EOF correto: O recetor (o read()) do read end p[0] só vê o EOF quando todas as referencias ao write end (p[1]) estiverem fechadas em todos os processos
									(incluindo o parent process). Se tiverem fechadas, o próximo read() devolve 0.
									Se esquecermos de um  write end p[1] aberto no parent process (ou noutro processo) por engano, o read end p[0] nunca chega a ver o EOF
									e pode ficar bloqueado para sempre à espera de dados que não virão.
			
		  > Evitar FDs a mais sem utilidade: Depois do dup2, o FD 1 (ou o FD 0) apontam para o pipe. Manter o p[0] ou o p[1] abertos, duplica as referências e
 											 pode alterar a semântica.
			
		  > Se não existe nenhum read end aberto (ou seja, todos os p[0] estão fecharados), o write falha e o processo recebe 
			SIGPIPE (por omissão termina).


		  > safe_dup2(infile, STDIN_FILENO)

		    Se o infile for um FD válido, o kernel fecha o STDIN (FD 0) (se estiver aberto) e faz o FD 0 apontar para o mesmo FD do infile.
	        Por exemplo, se o FD do infile é 3, com o dup2(), o FD 0 passa apontar para o FD 3, que é o FD do infile.

            No primeiro child, vamos ligar/redirecionar o STDIN (FD 0) ao FD do infile (infile_fd).
		    Assim, o STDIN do child 1 que antes apontava e lia do terminal, passa a ler do infile_fd (o ficheiro av[1]).

			Resultado: o stdin passa a ler do infile, pois o stdin (FD 0) aponta agora para o FD do infile (FD 0 -> infile_fd)

			Porquê que fechamos ?

			Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
			O FD infile já não será utilizado.
			Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
			Se não fecharmos o FD do infile e o processo sair por erro mais tarde, durante esse intervalo, o kernel
			continua a ver que ainda há um infile_fd aberto.
			Se outro processo abrir o mesmo ficheiro, pode receber o mesmo FD (o mesmo número inteiro) e causar confusão.
			Ao fechar imediatamente o infile_fd no erro, o número de referencias ao infile é decrementado.

		  > safe_dup2(pipefd[1], STDOUT_FILENO)

			O kernel fecha o STDOUT (FD 1) (se estiver aberto) e faz o STDOUT (FD 1) apontar para o mesmo FD que o write end do pipe.
			Por exemplo, se o FD do write end for 5, com o dup2(), o FD 1 passa a apontar para o FD 5, que é o FD do write end do pipe.

			No primeiro child, vamos ligar/redirecionar o STDOUT (FD 1) ao FD do write end do pipe (pipefd[1]).
			Assim, o STDOUT do child 1 que antes apontava e escrevia no terminal, passa a escrever no write end do pipe.

			Resultado: o stdout passa a escrever no pipe, pois o stdout (FD 1) aponta agora para o FD do write end do pipe (FD 1 -> pipefd[1]).

			Porquê que fechamos ?

			Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
			O write end pipefd[1] já não será utilizado.
			Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
			Se não fecharmos o write end do pipe e o processo sair por erro mais tarde, durante esse intervalo o kernel
			continua a ver que ainda há um escritor vivo.
			O leitor (o child 2) pode não receber o EOF quando deveria, causando um bloqueio (ele fica à espera eternamente).
			Ao fechar imediatamente o pipefd[1] no erro, o número de referencias ao write end do pipe é decrementado e assim, o 
			leitor (o child 2) pode ver o EOF no timming certo (quando os restantes escritores fecharem).


	      > Comparação do antes e depois do dup2():
			
			Antes:                                               Depois:

			FD 0 (STDIN)  -> aponta para o terminal              FD 0 (STDIN)  -> aponta para o infile_fd (O stdin agora lê do infile)
			FD 1 (STDOUT) -> aponta para o terminal              FD 1 (STDOUT) -> aponta para o pipefd[1] (O stdout agora escreve no pipe)
			FD 2 (STDERR) -> aponta para o terminal              FD 2 (STDERR) -> aponta para o terminal
			FD 3 -> aponta para o infile_fd                      FD 3 -> aponta para o infile_fd (original ainda continua aberto)
			FD 4 -> aponta para o pipefd[0] [FECHADO]            FD 4 -> aponta para o pipefd[0] [FECHADO]
			FD 5 -> aponta para o pipefd[1] (write end)          FD 5 -> aponta para o pipefd[1] (original ainda aberto)

		    O infile_fd e o pipefd[1] estão agora ligados aos FDs padrão 0 e 1.
		    O infile_fd e o pipefd[1] originais ainda estão abertos, mas já não são necessários.

		 a5) close(pipefd[1]); 
	         close(infile);

	         A seguir ao dup2() fechamos o write end do pipe (pipefd[1]) e o infile_fd, porque os FDs originais já não serão precisos.

	         Como vimos, depois do dup2() ficámos com duas referências para cada destino:

			 FD 0 (stdin) ─┐                          FD 1 (stdout) ─┐
			        	   └─► [infile_fd]                           └─► [pipefd[1] do pipe]
		      infile_fd   ─┘                              pipefd[1] ─┘

		
			 Fechamos os FDs originais, pois para o child 2 ver o EOF, o kernel precisa que todas as pontas de leitura do pipe 
			 estejam fechadas. Caso ficasse o pipefd[1] aberto para além do stdout (FD 1), haveria mais uma referencia de escrita,
			 o que atrasaria o EOF do child 2 (ele só vê o EOF quando todas as referencias de escrita fecharem). Fechando o pipefd[1],
			 ficamos com apenas o stdout (FD 1) como escritor, assim, quando o processo terminar, o EOF chega ao child 2.

			 Ao fechar: - evitamos leaks.
						- reduzimos os contadores do pipe (útil para EOF no leitor),

	         A tabela de FDs final ficará:  FD 0 (STDOUT) -> aponta para o infile_fd
									        FD 1 (STDIN)  -> aponta para o pipefd[1]

		  a6) ft_exec_cmd(av[2], envp);

			  Finalmente, chamamos a função ft_exec_cmd() para substituir o child 1 pelo comando av[2] (cmd1).
			  O cmd1 vai ler do infile (pois o stdin está ligado ao infile) e vai escrever no pipe (pois o stdout está ligado ao write end do pipe).

		  a7) close(pipefd[1]);
		  
		      No parent process, fechamos o write end do pipe (pipefd[1]), pois o parent não vai escrever no pipe - só o child process (do comando atual) escreve no pipe.
		      Se não fechassemos essa referência, o kernel entenderia que ainda havia um writer vivo/aberto e assim, quando o leitor (o próximo comando da cadeia) tentasse
			  ler do pipe, nunca veria o EOF, pois o kernel só envia o EOF quando todas as referencias ao write end do pipe estão fechadas.
			  Isto causa bloqueio eterno no processo leitor, mesmo que o parent e o child 1 já tenham morrido/terminado, pois ainda haveria um writer aberto (o pipefd[1] do parent).
			  
			  Assim, fechar o pipefd[1] no parent: - evita deadlocks/bloqueios;
			                                       - evita leak de FDs;
												   - garante que o kernel consiga gerir corretamente o EOF para o proximo comando assim que o child 1 terminar (o kernel só envia o EOF
													 quando todas as referencias ao write end do pipe estão fechadas).
			  
		  a8) *prev_readfd = pipefd[0];
		  
		      O parent process não fecha o read end do pipe (pipefd[0]), pois este é necessário para o próximo comando (o middle_child()).
			  O pipe no qual o first_child() escreveu será a entrada (input_fd) do próximo comando.
			  Para passar essa "entrada anterior" para o próximo comando, o parent guarda o FD do read end do pipe (pipefd[0]) na variável prev_readfd.
			  A variável prev_readfd representa "o read end da etapa anterior" (o input_fd que vem de trás).
			  
			  Assim, atualizamos o prev_readfd (input_fd) para ser o read end do novo pipe (pipefd[0]), que será usado como input_fd (STDIN) do próximo comando.
			  O prev_readfd é passado por referência para que na main() possamos atualizar o input_fd para o passarmos para o comando seguinte a ser executado.
			  Assim, o próximo comando vai ler deste pipefd[0].
			  
			  
		  	  Na main(), após chamar a função first_child(), fazemos:

			  i = 3;
			  while (i < ac - 2)
			  {
				last_pid = middle_child(av, &prev_read_fd, envp, i);
				i++;
			  }

			  Aqui, o prev_read_fd é passado por referência para que o middle_child() possa atualizar o input_fd
			  para o próximo comando.


			
				pid_t	middle_child(char **av, int *prev_readfd, char **envp, int i)
				{
					int in_fd;
					int pipefd[2];
					pid_t pid;

					in_fd = *prev_readfd;                    // herda o read end do pipe do primeiro comando (ou do comando (middle) anterior)
					get_pipe_and_fork(pipefd, &pid);         // cria um novo pipe (para ligar este comando ao proximo) e cria um novo child process (fork)
					if (pid == 0)
					{
						close(pipefd[0]);                     // fechamos o read end do child, pois ele não vai ler do pipe
						safe_dup2(in_fd, STDIN_FILENO);       // liga o input_fd (read end do pipe ou do middle anterior) ao STDIN do child
						safe_dup2(pipefd[1], STDOUT_FILENO);  // liga o write end do novo pipe (para o próximo comando) ao STDOUT do child
						close(pipefd[1]);                     // fechamos a referencia ao write end do child, pois já está ligado ao STDOUT (já está redirecionado).
						close(in_fd);                         // fechamos a referencia ao input_fd do child, pois já está ligado ao STDIN (já está redirecionado).
						ft_exec_cmd(av[i], envp);
					}
					close(in_fd);                             // PARENT: fecha a referencia ao input_fd do parent, pois já não é necessário (já foi passado ao child)
					close(pipefd[1]);                         // PARENT: fecha a referencia ao write end do parent, pois o parent não vai escrever no pipe (só o child escreve)
					*prev_readfd = pipefd[0];                 // PARENT: atualiza o input_fd (read end do pipe do middle) para o próximo comando
					return (pid);                             // PARENT: devolve o PID do child criado
				}

				a2) Chamamos a função get_pipe_and_fork() para criar o pipe e o criar o segundo processo (o segundo comando).

	      			get_pipe_and_fork(int pipefd[2], pid_t *pid)

		  			O kernel cria um objeto pipe com um buffer e devolve dois FDs abertos que apontam para as duas pontas do mesmo pipe:
	      
            		- pipefd[0]: read end -> aberto só para leitura do pipe: associado ao FD de leitura;
            		- pipefd[1]: write end -> aberto só para escrita no pipe: associado ao FD de escrita;
  
          			O fork() cria um novo processo (child 2) que é uma cópia do parent.
		            O child herda a mesma tabela de FDs do parent, ou seja, o child 2 também tem o pipefd[0] e o pipefd[1] abertos.

		  			Depois do fork(), tanto o child como o parent têm as duas pontas do pipe abertas:

	  	  			Parent (após o fork):  FD 3 -> pipefd[0]
					         	           FD 4 -> pipefd[1]

		  			Child 1 (cópia):  FD 3 -> pipefd[0]
					                  FD 4 -> pipefd[1]
		  
		  			O fork() devolve o PID do child no parent (pid > 0) e devolve 0 no pid do child (pid == 0).

					Por que criar o pipe antes do fork?
					
					Para que tanto o parent quanto o child possuam os FDs do pipe e cada um feche o que não usa. 
					Isso é essencial para a correta sinalização do EOF e para evitar deadlocks.
							
				    Este novo pipe criado servirá para transportar o STDOUT deste comando (cmd2) para o STDIN do próximo (cdm3).
					O pipe faz isso ao ligar o STDOUT deste comando (cmd2) ao STDIN do próximo comando (cmd3).
				
				a3) in_fd = *prev_readfd;
				
					O in_fd recebe o valor atual do *prev_readfd, que é o input_fd passado por referência - o input que veio do first_child().
				    O in_fd é o read end do pipe do comando anterior (first_child) - o pipe no qual o first_child() escreveu.

				a4) Se pid == 0, entramos no child 2 (que vai ser o nosso cmd2).
					Começamos por fechar o read end do pipe (pipefd[0]) neste processo, pois este processo não vai ler do read end do pipe, só 
					vai escrever no write end do pipe (o processo que vai ler (o leitor) vai ser o comando seguinte a este).

					O kernel mantém os contadores: o número de pontas de leitura que estão abertas (pipefd[0]) e o número de pontas de
					escrita que estão abertas (pipefd[1]) do pipe.
					Se o processo que vai escrever no pipe (o escritor, que neste caso é o child 2) mantiver aberta a read end (pipefd[0]), então o 
					número de referencias ao read end será maior do que 0 (> 0) - ou seja, o número de pontas de leitura será > 0 - mesmo que o 
					verdadeiro processo que vai ler do pipe (o leitor, que é o child 3) já tenha fechado/morrido.
					Assim, quando o processo leitor real fechar, o processo escritor continua a escrever até encher o buffer, e então bloqueia
					para sempre, pois ninguém está a consumir (ler).

					-> Fechar o read end (pipefd[0]) no child 2 faz com que o número de pontas de leitura abertas (pipefd[0]) seja a correta,
					   assim, quando o child 3 morrer, esse número será 0 e write no child 2 já não bloqueia.
					
					-> Ao fechar o read end, garantimos que o contador de referências do read end do pipe está correto.

				a5) safe_dup2(in_fd, STDIN_FILENO)

					Se o in_fd for um FD válido, o kernel fecha o STDIN (FD 0) (se estiver aberto) e faz o FD 0 apontar para o mesmo FD do in_fd.
					Por exemplo, se o FD do in_fd é 3, com o dup2(), o FD 0 passa apontar para o FD 3, que é o FD do in_fd.

					No segundo child, vamos ligar/redirecionar o STDIN (FD 0) ao FD do in_fd, que é o read end do pipe do comando anterior (pipefd[0]).
					Assim, o STDIN do child 1 que antes apontava e lia do terminal, passa a ler do in_fd (o read end do pipe do comando anterior).

					Resultado: o stdin passa a ler do read end do pipe do comando anterior, pois o stdin (FD 0) aponta agora para o FD do in_fd (FD 0 -> in_fd)

					Porquê que fechamos ?

					Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
					O FD in_fd já não será utilizado.
					Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
					Se não fecharmos o read end do pipe do comando anterior e o processo sair por erro mais tarde, durante esse intervalo o kernel
					continua a ver que ainda há um leitor vivo.
					O processo escritor (o child 1) pode não receber o SIGPIPE quando deveria, causando um bloqueio (ele fica à espera eternamente).
					Ao fechar imediatamente o in_fd no erro, o número de referencias ao read end do pipe é decrementado e assim , o
					processo escritor (o child 1) pode ver o SIGPIPE no timming certo (quando os restantes leitores fecharem).
					
				a6) safe_dup2(pipefd[1], STDOUT_FILENO)

					O kernel fecha o STDOUT (FD 1) (se estiver aberto) e faz o STDOUT (FD 1) apontar para o mesmo FD que o write end do pipe.
					Por exemplo, se o FD do write end for 5, com o dup2(), o FD 1 passa a apontar para o FD 5, que é o FD do write end do pipe.

					No segundo child, vamos ligar/redirecionar o STDOUT (FD 1) ao FD do write end do pipe (pipefd[1]).
					Assim, o STDOUT do child 2 que antes apontava e escrevia no terminal, passa a escrever no write end do pipe.

					Resultado: o stdout passa a escrever no pipe, pois o stdout (FD 1) aponta agora para o FD do write end do pipe (FD 1 -> pipefd[1]).

					Porquê que fechamos ?

					Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
					O write end pipefd[1] já não será utilizado.
					Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
					Se não fecharmos o write end do pipe e o processo sair por erro mais tarde, durante esse intervalo o kernel
					continua a ver que ainda há um escritor vivo.
					O leitor (o child 2) pode não receber o EOF quando deveria, causando um bloqueio (ele fica à espera eternamente).
					Ao fechar imediatamente o pipefd[1] no erro, o número de referencias ao write end do pipe é decrementado e assim, o 
					leitor (o child 2) pode ver o EOF no timming certo (quando os restantes escritores fecharem).


				> Comparação do antes e depois do dup2():
					
					Antes:                                               		Depois:

					FD 0 (STDIN)  -> aponta para o terminal              		FD 0 (STDIN)  -> aponta para o in_fd (O stdin agora lê do in_fd, ou seja do read end)
					FD 1 (STDOUT) -> aponta para o terminal              		FD 1 (STDOUT) -> aponta para o pipefd[1] (O stdout agora escreve no pipe)
					FD 2 (STDERR) -> aponta para o terminal              		FD 2 (STDERR) -> aponta para o terminal
					FD 3 -> aponta para o in_fd (read end do comando anterior)  FD 3 -> aponta para o in_fd (original ainda continua aberto)
					FD 4 -> aponta para o pipefd[0] [FECHADO]            		FD 4 -> aponta para o pipefd[0] [FECHADO]
					FD 5 -> aponta para o pipefd[1] (write end)          		FD 5 -> aponta para o pipefd[1] (original ainda aberto)

					O in_fd e o pipefd[1] estão agora ligados aos FDs padrão 0 e 1.
					Como há essas novas referências para o STDIN e para o STDOUT, podemos fechar as referências originais (in_fd e pipefd[1]), pois o child process já não 
					precisa dessas referencias originais (pois já estão ligadas ao STDIN e ao STDIN).
					
					O in_fd e o pipefd[1] originais ainda estão abertos, mas já não são necessários.
				
				
				a7) close(pipefd[1]); 
	         		close(in_fd);

					A seguir ao dup2() fechamos o write end do pipe (pipefd[1]) e o infile_fd, porque os FDs originais já não serão precisos.

					Como vimos, depois do dup2() ficámos com duas referências para cada destino:

					FD 0 (stdin) ─┐                          FD 1 (stdout) ─┐
								  └─► [in_fd]                               └─► [pipefd[1] do pipe]
					    in_fd    ─┘                              pipefd[1] ─┘

				
					Fechamos os FDs originais, pois para o child 3 ver o EOF, o kernel precisa que todas as pontas de leitura do pipe 
					estejam fechadas. Caso ficasse o pipefd[1] aberto para além do stdout (FD 1), haveria mais uma referencia de escrita,
					o que atrasaria o EOF do child 3 (ele só vê o EOF quando todas as referencias de escrita fecharem). Fechando o pipefd[1],
					ficamos com apenas o stdout (FD 1) como escritor, assim, quando o processo terminar, o EOF chega ao child 3.

					Ao fechar: - evitamos leaks.
								- reduzimos os contadores do pipe (útil para EOF no leitor),

					A tabela de FDs final ficará:   FD 0 (STDOUT) -> aponta para o in_fd (o read end do comando anterior)
													FD 1 (STDIN)  -> aponta para o pipefd[1]

				a7) Finalmente, chamamos a função ft_exec_cmd() para substituir o child 2 pelo comando av[i] (que neste primeiro caso será o av[3]).
				    A partir daqui, o processo torna-se o binário do comando e usa os FDs redirecionados (STDIN e STDOUT ligados aos pipes).
					O cmd2 vai ler do in_fd, ou seja, do read end do pipe do comando anterior (cmd1), pois o stdin está ligado ao in_fd, e vai escrever 
					no write end do pipe (pois o stdout está ligado ao write end do pipe).
					
				    Se a execução do comando falhar, a função ft_exec_cmd() termina o processo com error_exit().

				a8) No parent process (pid > 0), fechamos a referência ao in_fd (read end do pipe do heredoc ou do middle anterior), pois o parent já não precisa dele.
				    Essa referência já foi passada ao child process (do comando atual) .
					Se não fechassemos essa referência, haveria um leak de FDs e o kernel não conseguiria gerir corretamente o EOF para o proximo comando.

					close(in_fd);

				a9) close(in_fd);
				    close(pipefd[1]);
				
					No parent process (pid > 0), fechamos a referencia ao in_fd (que é o read end do pipe do comando anterior), pois o parent já não precisa dele.
					Essa referencia já foi passada ao child.
					
					Fechamos, também, o write end do pipe (pipefd[1]), pois o parent não vai escrever no pipe - só o child process (do comando atual) escreve no pipe.
		      		Se não fechassemos essa referência, o kernel entenderia que ainda havia um writer vivo/aberto e assim, quando o leitor (o próximo comando da cadeia) tentasse
			  		ler do pipe, nunca veria o EOF, pois o kernel só envia o EOF quando todas as referencias ao write end do pipe estão fechadas.
			  		Isto causa bloqueio eterno no processo leitor, mesmo que o parent e o child 1 já tenham morrido/terminado, pois ainda haveria um writer aberto (o pipefd[1] do parent).
			  
			  		Assim, fechar o pipefd[1] no parent: - evita deadlocks/bloqueios;
			                                      		 - evita leak de FDs;
												  		 - garante que o kernel consiga gerir corretamente o EOF para o proximo comando assim que o child 1 terminar (o kernel só envia o EOF
													 	   quando todas as referencias ao write end do pipe estão fechadas).

				a10) *prev_readfd = pipefd[0];
		  
					O parent process não fecha o read end do pipe (pipefd[0]), pois este é necessário para o próximo comando (outro middle_child() ou o last_child()).
					O pipe no qual o middle_child() escreveu será a entrada (input_fd) do próximo comando.
					Para passar essa "entrada anterior" para o próximo comando, o parent guarda o FD do read end do pipe (pipefd[0]) na variável prev_readfd.
					A variável prev_readfd representa "o read end da etapa anterior" (o input_fd que vem de trás).
					
					Assim, atualizamos o prev_readfd (input_fd) para ser o read end do novo pipe (pipefd[0]), que será usado como input_fd (STDIN) do próximo comando.
					O prev_readfd é passado por referência para que na main() possamos atualizar o input_fd para o passarmos para o comando seguinte a ser executado.
					Assim, o próximo comando vai ler deste pipefd[0].

					Na pipeline, o parent process vai repetir este padrão: mantém o "fio" do read end do pipe (pipefd[0]) para o próximo comando, passando-o por referência, e assim, cada
					middle deixa preparado o read end para ser o in_fd seguinte.

				a11) Finalmente, o parent process devolve o PID do child process criado (do comando atual), para que o chamador possa fazer wait().

					 return (pid);


				Na main(), após o while que chama o middle_child(), fazemos:		
					 
				pid_t	last_child(int ac, char **av, int prev_readfd, char **envp)
				{
					int	outfile_fd;
					pid_t pid;

					pid = fork();
					if (pid < 0)
						error_exit("Fork Failed!");
					if (pid == 0)
					{
						outfile_fd = open(av[ac - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
						if (outfile_fd == -1)
						{
							close(prev_readfd);
							error_exit("Error on output file");
						}
						safe_dup2(prev_readfd, STDIN_FILENO);
						safe_dup2(outfile_fd, STDOUT_FILENO);
						close(prev_readfd);
						close(outfile_fd);
						ft_exec_cmd(av[ac - 2], envp);
					}
					close(prev_readfd);
					return (pid);
				}

				a1) O fork() cria um novo processo (child 2) que é uma cópia do parent.
		            O child herda a mesma tabela de FDs do parent, ou seja, o child 3 também tem o pipefd[0] e o pipefd[1] abertos.

		  			Depois do fork(), tanto o child como o parent têm as duas pontas do pipe abertas:

	  	  			Parent (após o fork):  FD 3 -> pipefd[0]
					         	           FD 4 -> pipefd[1]

		  			Child 3 (cópia):  FD 3 -> pipefd[0]
					                  FD 4 -> pipefd[1]
		  
		  			O fork() devolve o PID do child no parent (pid > 0) e devolve 0 no pid do child (pid == 0).

				a2) Se pid == 0, entramos no child 3 (que vai ser o nosso last command).
					Começamos por abrir o ficheiro de output - o nosso outfile - (av[ac - 1]) em modo truncate (O_TRUNC), ou seja, se o ficheiro
					já existir, o kernel apaga todo o conteúdo do ficheiro e começa a escrever do início.
					Se o ficheiro não existir, o kernel cria-o (O_CREAT).

					outfile_fd = open(av[ac - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

					O_WRONLY: abre o ficheiro apenas para escrita. O último comando vai escrever o output nesse ficheiro.

					O_CREAT: se o ficheiro não existir, o kernel cria-o. É por isso que passamos também o modo 0644 (permissões) como terceiro
					         argumento do open(), que será aplicado só no caso de criação do ficheiro.

					O_TRUNC: se o ficheiro já existir, o kernel apaga todo o conteúdo do ficheiro e começa a escrever do início.
					         Assim, o output do último comando substitui todo o conteúdo anterior do ficheiro.
							 Corresponde ao operador > no shell.

					0644: são as permissões do ficheiro que se aplicam apenas no caso de ele ser criado (O_CREAT). 
					      rw-r--r-- ->  O dono pode ler e escrever (6 = 4 + 2), o grupo e os outros podem apenas ler (4).

					Se a abertura do ficheiro falhar (por exemplo, por falta de permissões), o open() devolve -1 e fechamos o prev_readfd e terminamos com error_exit().

					A função open() abre um ficheiro e devolve um FD.
					O kernel vai percorrer o ficheiro (av[ac - 1]) e verifica, com o O_WRONLY, se esse ficheiro tem permissão só de escrita.
				    Se tudo estiver OK, é devolvido um FD novo (um número inteiro >= 0) que será armazenado no outfile_fd.
					
					Se o open() falhar ao abrir o ficheiro outfile (-1) - por falta de permissão - o child 3 não irá conseguir ligar/redirecionar o STDIN ao FD do
					outfile, para assim escrever no ficheiro.
					Assim, fechamos o prev_readfd (o read end do pipe do middle anterior), pois se não fechassemos o prev_readfd aqui, o processo escritor (o middle anterior)
					poderia ficar bloqueado à espera de escrever mais dados no write end do pipe ou receber o SIGPIPE (pois haveria ainda um leitor aberto, o prev_readfd
					do child 3), mesmo que o parent e o middle anterior já tivessem morrido.
					Saímos com error_exit() para terminar o processo com o código de erro apropriado.

					> Porquê que precisamos de abrir o outfile ?

					  Porque o último comando da pipeline vai escrever o output nesse ficheiro e, assim, ele precisa de um destino para o STDOUT.
					  O último comando (av[ac - 2]) escreve o output no STDOUT, mas como redirecionámos o STDOUT para o ficheiro (output_fd), o output vai para o ficheiro.
					  Se não abrissemos o outfile, não teriamos um FD válido para ligar o STDOUT do último comando e o redirecionamento falharia.
					  Assim, o open() devolve um FD válido (output_fd) que podemos usar para ligar o STDOUT do último comando, com o dup2().
					  Se o ficheiro não existir, o kernel cria-o (O_CREAT).
					  Se a abertura do ficheiro falhar (por exemplo, por falta de permissões), o open() devolve -1 e fechamos o prev_readfd e terminamos com error_exit().


				a3) safe_dup2(prev_readfd, STDIN_FILENO)

					Se o prev_readfd for um FD válido, o kernel fecha o STDIN (FD 0) (se estiver aberto) e faz o FD 0 apontar para o mesmo FD do prev_readfd.
					Por exemplo, se o FD do in_fd é 3, com o dup2(), o FD 0 passa apontar para o FD 3, que é o FD do prev_readfd.

					No último child, vamos ligar/redirecionar o STDIN (FD 0) ao FD do prev_readfd, que é o read end do pipe do comando anterior.
					Assim, o STDIN do último child que antes apontava e lia do terminal, passa a ler do prev_readfd (o read end do pipe do comando anterior).

					Resultado: o stdin passa a ler do read end do pipe do comando anterior, pois o stdin (FD 0) aponta agora para o FD do prev_readfd (FD 0 -> in_fd)

					Porquê que fechamos ?

					Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
					O FD prev_readfd já não será utilizado.
					Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
					Se não fecharmos o read end do pipe do comando anterior e o processo sair por erro mais tarde, durante esse intervalo o kernel
					continua a ver que ainda há um leitor vivo.
					O processo escritor (o child 1) pode não receber o SIGPIPE quando deveria, causando um bloqueio (ele fica à espera eternamente).
					Ao fechar imediatamente o in_fd no erro, o número de referencias ao read end do pipe é decrementado e assim , o
					processo escritor (o child 1) pode ver o SIGPIPE no timming certo (quando os restantes leitores fecharem).
					
				a6) safe_dup2(outfile, STDOUT_FILENO)

					O kernel fecha o STDOUT (FD 1) (se estiver aberto) e faz o STDOUT (FD 1) apontar para o mesmo FD que o outfile.
					Por exemplo, se o FD do write end for 5, com o dup2(), o FD 1 passa a apontar para o FD 5, que é o FD do ficheiro outfile.

					No último child, vamos ligar/redirecionar o STDOUT (FD 1) ao FD do outfile.
					Assim, o STDOUT do ultimo child que antes apontava e escrevia no terminal, passa a escrever no ficheiro outfile.

					Resultado: o stdout passa a escrever no ficheiro outfile, pois o stdout (FD 1) aponta agora para o FD do outfile.

					Porquê que fechamos ?

					Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
					O outfile já não será utilizado.
					Ao fechá-lo, libertamos a posição da tabela de FDs do processo.

					> Comparação do antes e depois do dup2():
					
					Antes:                                               		      Depois:

					FD 0 (STDIN)  -> aponta para o terminal              		      FD 0 (STDIN)  -> aponta para o prev_readfd (O stdin agora lê do prev_readfd, ou seja do read end)
					FD 1 (STDOUT) -> aponta para o terminal              		      FD 1 (STDOUT) -> aponta para o outfile_fd (O stdout agora escreve no outfile)
					FD 2 (STDERR) -> aponta para o terminal              		      FD 2 (STDERR) -> aponta para o terminal
					FD 3 -> aponta para o prev_readfd (read end do comando anterior)  FD 3 -> aponta para o prev_readfd (original ainda continua aberto)
					FD 4 -> aponta para o outfile_fd           						  FD 4 -> aponta para o outfile_fd (original ainda aberto)

					O prev_readfd e o outfile_fd estão agora ligados aos FDs padrão 0 e 1.
					Como há essas novas referências para o STDIN e para o STDOUT, podemos fechar as referências originais (prev_readfd e outfile_fd), pois o child process já não 
					precisa dessas referencias originais (pois já estão ligadas ao STDIN e ao STDOUT).

				a7) close(prev_readfd);
	         		close(outfile_fd);

					A seguir ao dup2() fechamos o prev_readfd (read end do pipe do middle anterior) e o outfile_fd, porque os FDs originais já não serão precisos.

					Como vimos, depois do dup2() ficámos com duas referências para cada destino:

					FD 0 (stdin) ─┐                          FD 1 (stdout) ─┐
								  └─► [prev_readfd]                         └─► [outfile_fd]
					 prev_readfd ─┘                             outfile_fd ─┘

				
					Fechamos os FDs originais, pois para o último child ver o EOF, o kernel precisa que todas as pontas de leitura do pipe 
					estejam fechadas. Caso ficasse o prev_readfd aberto para além do stdout (FD 1), haveria mais uma referencia de escrita,
					o que atrasaria o EOF do último child (ele só vê o EOF quando todas as referencias de escrita fecharem). Fechando o prev_readfd,
					ficamos com apenas o stdout (FD 1) como escritor, assim, quando o processo terminar, o EOF chega ao último child.

					Ao fechar: - evitamos leaks.
							   - reduzimos os contadores do pipe (útil para EOF no leitor),

					A tabela de FDs final ficará:   FD 0 (STDOUT) -> aponta para o prev_readfd (o read end do comando anterior)
													FD 1 (STDIN)  -> aponta para o outfile_fd


				a8) Finalmente, chamamos a função ft_exec_cmd() para substituir o último child pelo comando av[ac - 2] (o último comando).
				    A partir daqui, o processo torna-se o binário do comando e usa os FDs redirecionados (STDIN e STDOUT ligados aos pipes e ao ficheiro).
					O last command vai ler do prev_readfd, ou seja, do read end do pipe do comando anterior (cmdN-1), pois o stdin está ligado ao prev_readfd,
					e vai escrever no outfile (pois o stdout está ligado ao outfile).
					
				    Se a execução do comando falhar, a função ft_exec_cmd() termina o processo com error_exit().

				
				a9) No parent process (pid > 0), fechamos a referência ao prev_readfd (read end do pipe do comando anterior), pois o parent já não precisa dele.
				    Essa referência já foi passada ao child process (do comando atual) .
					Se não fechassemos essa referência, haveria um leak de FDs e o kernel não conseguiria gerir corretamente o EOF para o proximo comando.

					close(prev_readfd);

				a10) Finalmente, o parent process devolve o PID do child process criado (do comando atual), para que o chamador possa fazer wait().

					 return (pid);
				
				
				

	
				pid_t ft_heredoc_pipeline(int ac, char **av, char **envp)
				{
					pid_t last_pid;
					pid_t hdoc_pid;
					int input_fd;
					int i;

					if (ac < 6)      // heredoc + LIMITER + >= 2 cmds + outfile
						show_usage_exit2();
					input_fd = -1;
					hdoc_pid = ft_heredoc(av, &input_fd);
					i = 3;
					last_pid = -1;
					if (i < ac - 2)       // se há pelo menos um middle antes do last
					{
						last_pid = middle_child(av, &input_fd, envp, i++);
						waitpid(hdoc_pid, NULL, 0);
						while (i < ac - 2)
							last_pid = middle_child(av, &input_fd, envp, i++);
						last_pid = exec_last_and_append(ac, av, input_fd, envp);
					}
					else   // se não houver middles, vai direto para o último
						last_pid = exec_last_and_append(ac, av, input_fd, envp);
					return (last_pid);
				}

				> O que é um heredoc (here-document) ?

				Um heredoc é um modo do shell fornecer dados para a entrada (STDIN) de um comando, sem utilizar
				um ficheiro pré-existente. Em vez disso, escrevemos o conteúdo no próprio terminal, e esse conteúdo é entregue
				ao comando até o shell encontrar uma linha que seja exatamente o limitador (LIMITER), seguida de uma \n.
				
				O heredoc é, assim, um mecanismo pelo o qual o shell substitui o STDIN de um determinado comando por um fluxo de 
				entrada de dados que contém o texto que escrevemos no próprio terminal, até à linha que contém o delimitador (LIMITER),
				seguida de uma \n.

				cat << END
				linha 1
				linha 2 
				END

				Aqui, tudo o que estiver entre o << LIMITER e a linha exata LIMITER (seguida de uma \n) é enviado para o STDIN 
				do comando cat. O fluxo de entrada dos dados termina quando o shell encontra uma linha que é exatamente o delimitador
				END (seguida de \n).

				> O que é o simbolo '<<' ?

				O simbolo '<<' introduz um heredoc, ativando-o.
				O shell utiliza o simbolo '<<' para criar um fluxo de entrada de dados para um determinado comando, a partir do próprio
				terminal, captando linha a linha, até encontrar a string delimitadora (LIMITER). 
				Assim, o shell vai redirecionar o STDIN do comando que está imediatamente à esquerda do '<<' para ler esse fluxo de dados.

				> O que é o simbo '>>' ?

				O simbolo '>>' é a redireção de saída com append.
				Ele abre (ou cria) um ficheiro, no fim, e redireciona (liga) o STDOUT do comando (imediatamente à esquerda) a
				esse ficheiro, acrescentando ao fim, sem trucar.

				
				Como já sabemos, o pipe '|' liga a a saída (stdout) de um comando à entrada (stdin) do comando seguinte.
					
				Em cmd1 | cmd2, o cmd1 escreve no fd do write end do pipe (stdout->pipefd[1]) e o cmd2 lê do fd do read end do 
				pipe (stdin->pipefd[0]).
					
				No caso de cmd1 << LIMITER | cmd2, o heredoc abastace o STDIN do cmd1 com dados, e o STDOUT do cmd1 vai para o 
				STDIN do cmd2, através do pipe.

				cmd1 << LIMITER | cmd2 | ... | cmdN >> outfile

				Aqui, o heredoc abastace o STDIN do cmd1.
				A saída (STDOUT) do cmd1 é redirecionada para a entrada (STDIN) do cmd2, via pipe.
				O último comando (cmdN) tem a saída (STDOUT) redirecionada ao outfile (>> outfile). 

				
				> Pipeline do heredoc
				
				static pid_t    ft_heredoc(char **av, int *hdoc_readfd)
				{
					int pipefd[2];
					pid_t pid;

					get_pipe_and_fork(pipefd, &pid);
					if (pid == 0)
						read_till_limiter(av, pipefd);
					close(pipefd[1]);
					*hdoc_readfd = pipefd[0];
					return (pid);
				}

				Aqui criamos o pipe e o produtor do heredoc.

				1) get_pipe_and_fork(pipefd, &pid)

				O pipe(pipefd) cria um objeto pipe no kernel com duas pontas:  - pipefd[0] → read end
																				- pipefd[1] → write end

				O fork() duplica o processo, criando um child. 
				Ambos os processos (parent e child) têm cópias dos dois FDs: - o pipefd[0] e o pipefd[1].
				
				- O child recebe o pid == 0
				- O parent recebe o pid == <PID do filho> (pid > 0)
				
				2) read_till_limiter(char **av, int pipefd[2])
				
				Se o pid == 0, entramos no child process e chamamos a funçao read_till_limiter().
				O child fica responsável por ler do STDIN do utilizador/terminal e vai escrever no fd do write end (pipefd[1])
				do pipe até ao LIMITER. Este child é o chamado "produtor" do heredoc.

				static void read_till_limiter(char **av, int pipefd[2])
				{
					char *line;
					char *limiter;

					limiter = av[2];
					close(pipefd[0]);                                                       // este processo não lê do pipe
					while (1)
					{
						write(STDOUT_FILENO, "> ", 2);                                      // prompt
						line = get_next_line(STDIN_FILENO);                                 // lê uma linha do teclado/STDIN
						if (!line)                                                          // Chegou ao EOF (CTRL + D)
							return (warn_heredoc_eof(limiter), close(pipefd[1]), exit(0));
						if (remove_newline(line, limiter))                                  // Se for o LIMITER (sem \n), termina
						{
							free(line);
							break ;
						}
						write(pipefd[1], line, ft_strlen(line)); // aqui a line está sem a \n, então escrevemos a linha ...
						write(pipefd[1], "\n", 1);               // e depois escrevemos a \n .
						free(line);
					}
					close(pipefd[1]);                            // fecha o write end para sinalizar o EOF para o processo leitor
					exit(0);
				}

				Aqui estamos dentro do child produtor do heredoc (é o processo que recolhe as linhas do terminal/utilizador e 
				escreve-as no write end do pipe).
				ELe vai escrever no write end (pipefd[0]) do pipe aquilo que o utilizador digitar, até ao LIMITER (av[2]).
				O parent process (ou seja, o primeiro comando da pipeline) vai ler pelo read end (pipefd[0]).

				> line é um ponteiro para uma string alocada pela função get_next_line() e corresponde a uma linha do terminal.
				> limiter é um ponteiro que aponta para a string delimitadora passada no argv (LIMITER).

				1) Fechar o read end - close(pipefd[0])

				Este processo não vai ler do fd do read end do pipe, ele só vai escrever no fd do write end (pipefd[1]).
				Ao fecharmos o read end neste processo, evitamos leaks de FDs e ajuda o kernel a sinalizar corretamente o EOF ao 
				processo leitor (cmd1) quando o produtor finalmente fechar o write end, evitando bloqueios.

				2) Loop principal

					while (1)
					{
						write(STDOUT_FILENO, "> ", 2);
						line = get_next_line(STDIN_FILENO);
						...
					}

					> Imprimimos o prompt "> ", com write(STDOUT_FILENO, "> ", 2).

					> Porque que usamos o GNL no heredoc ?

					No modo heredoc, nós queremos ler do utilizador/terminal uma linha inteira de cada vez até encontrarmos o LIMITER.
					Testamos se cada linha é o LIMITER e, se não for, envia essa linha ao write end do pipe.
					
					O STDIN_FILENO (= 0) é a entrada padrão, assim, o STDIN do processo child produtor do heredoc aponta para a saída 
					padrão, neste caso, aponta para o teclado/terminal (tty).
					Assim, a fonte do fluxo de entrada de dados é o terminal/teclado e não o read end do pipe.

					
					> O que é o EOF (end of file) ?

						EOF é o sinal de que não há mais dados para ler de um FD.
						Quando uma leitura retorna 0 bytes, isso significa EOF.
						
						- Num ficheiro, o EOF acontece quando já lemos todos os bytes até ao fim.
						- NUm pipe, o processo leitor vê o EOF quando todos os processos escritores dessa ponta fecharem o write end.
						- NUm terminal (tty), o Ctrl + D no inicio da linha, envia o EOF, sinalizando que não há mais input.
						Se houver texto digitado sem o ENTER, o kernel entrega esse texto como última linha sem a newline \n e depois 
						na chamada seguinte, atinge-se o EOF.

						
					A função get_next_line() utiliza a função read(fd, buf, BUFSIZE) quantas vezes forem precisas, para uma linha de cada vez. 
					A função GNL, devolve exatamente uma linha lida do FD, por cada chamada, que é o que queremos exatamente para o heredoc (ler até
					ao LIMITER linha por linha):

						- Se já jouver uma newline \n disponível, devolve do ínicio até à newline \n incluida (dessa linha);
						- Se não houver newline \n mas houver dados (que depois na próxima chamada vem o EOF), devolve a última linha sem a
						newline \n;
						- Se não houver mais nada para ler (atingiu o EOF), devolve NULL.
					
					A função get_next_line() utiliza uma stash ("leftover") que é um buffer onde ficam as sobras das leituras entre as
					chamadas, pois o read() traz os dados em blocos arbitrários (tamanho BUFFER_SIZE) e às vezes a newline \n encontra-se
					no meio desse bloco e sobram bytes depois da newline encontrada que pertencem à proxima linha.
					Para esses bytes não serem perdidos, vão para a stash.

											[fd] -> stash: "meia_linha\nresto_da_proxima"
																^            ^
																|            └─ sobra (vai ficar para a próxima chamada)
																|
																└─ linha completa para devolver (até e incluindo '\n')

					Quando pára de ler ? 
					
					- Quando encontrar a newline \n -> pára de ler -> corta a linha até à newline \n incluida e guarda-a na stash.
					- QUando chega ao EOF (o read() devolve 0) sem a newline \n -> get_next_line() devolve o que estiver na stash (sem a \n) e 
						limpa a stash, ficando vazia -> NULL.
						
					Assim, a função get_next_line() retorna NULL quando não há mais nada para ler (EOF e a stash vazia).

					
					(teclado) --STDIN--> [ filho heredoc ] ---- get_next_line() -----> devolve "texto\n" ----- testa se é LIMITER
																														|
																											escreve no write end do pipe
																														|
																														v
																											[ cmd1 lê do read end do pipe ]
								
					Propriedades úteis do GNL: - Como o STDIN do processo child produtor do heredoc aponta para a saída padrão (está ligado ao
													teclado/terminal (tty)), a função read() só entrega a linha completa quando o utilizador carregar 
													no ENTER.
												- Lida com linhas grandes (pode fazer várias leituras), pois se a linha for maior que o BUFFER_SIZE,
													a GNL faz várias leituras e continua a montar até à newline \n.
													No caso da última linha (sem a newline \n), se a entrada terminar sem a newline \n, a GNL devolve
													o que tiver -> isto é útil quando é dado o EOF (Ctrl + D), sem o ENTER.
												- Não perdemos bytes. A stash garante que nada se perde quando a newline \n se encontra no meio do bloco.
					
				> EOF no input (Ctrl + D)

					if (!line)
						return (warn_heredoc_eof(limiter), close(pipefd[1]), exit(0));

					Aqui, tratamos o fim do fluxo de entrada de dados durante o heredoc.
					A chamada da função get_next_line() tenta ler uma linha do terminal (STDIN).
					Ela devolve: - a string alocado com a linha (normalmente termina em newline \n);
								- ou NULL se não houver mais dados (atingiu o EOF) ou se ocorreu erro.

					Quando line == NULL ?

						- EOF no terminal: o utilizador carregou no CTRL + D no ínicio da linha (não havia nada pendente para entregar).
										O kernel sinaliza o EOF e o read() devolve 0 -> o GNL devolve NULL.
										Nota: se o utilizador carrega no CTRL + D mas há caracteres digitados (sem ENTER), primeiro a 
										GNL devolve essa última linha (sem a newline \n), e só depois numa nova chamada devolve NULL.
										Ou seja line == NULL só acontece quando não há mais nada a entregar.
						- STDIN fechado.
						- Erro de leitura ou de alocação.

					Assim, !line quer dizer que o GNL devolveu NULL → Chegou ao EOF antes de encontrar o LIMITER.

					Neste caso: - Imprimimos a mensagem de aviso para explicar que o utilizar terminou o input sem escrever o LIMITER.
								- Fecha o write end do pipe (pipefd[1]). Isto é fundamental, pois ao fechar o write end, o processo leitor
								(o cmd1, que tem o STDIN ligado ao pipefd[0]) vê o EOF (o read devolve 0) e termina a leitura normalmente.
								Se não fechassemos aqui o write end, o processo leitor poderia ficar bloqueado para sempre à espera de mais dados.
								- Terminamos imediatamente o processo produtor do heredoc com o código de saída 0 - exit(0).

					
				> Verificar o LIMITER (fim "normal" do heredoc)

					Após lermos a linha completa do terminal/teclado (e se line não for NULL), vamos verifcar se o delimitador LIMITER está presente nessa
					linha:

					if (remove_newline(line, limiter))
					{
						free(line);
						break;
					}

					Chamamos a função remove_newline():

					static int	remove_newline(char *line, char *delimiter)
					{
						size_t	len;

						if (!line || !delimiter)
							return (0);
						len = ft_strlen(line);
						if (len > 0 && line[len - 1] == '\n')
							line[len - 1] = '\0';
						return (ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0);
					}

					O objetivo aqui é retirar a newline \n (se existir) no final da line para verificar se a line é exatamente igual ao delimiter:

					ft_strncmp(line, delimiter, ft_strlen(delimiter) + 1) == 0;

					Começamos por verificar se line ou o delimiter são NULL, pois sem a line ou sem o delimiter não há comparação útil a realizar e,
					assim, devolvemos 0 (não é o delimitador).

					if (!line || !delimiter)
						return (0);

					De seguida, verificamos se a string tem pelo menos um caracter e se o último caracter é uma newline \n.
					Se for verdade, substituímos a newline \n pelo caractere nulo \0, retirando a newline no final da line.

					if (len > 0 && line[len - 1] == '\n')
						line[len - 1] = '\0';

					Transformamos, por exemplo, "LIMITER\n" em "LIMITER\0".
					A maior parte das linhas lidas pela funçaõ get_next_line() têm a newline no fim, mas o delimiter ("LIMITER") não tem newline \n.
					Caso não retirassemos a newline \n, a comparação falharia sempre.
					
					Por fim, comparamos a line com o delimiter até ft_strlen(delimiter) + 1 bytes (incluindo o '\0' final), o que garante que a
					comparação é exata, comparando, assim, o byte '\0' do delimiter.

					Porquê que comparamos? Porque exige igualdade exata: - line = "END\0" vs delimiter = "END\0" → iguais (retorna 1).
																		- line = "ENDX\0" vs delimiter = "END\0" → diferem no 4.º byte ('\0' vs 'X') → não iguais.
																		
					Assim: - Retorna 1 se a linha é o delimitador.
							- Retorna 0, se a linha não é o delimitador.

					Isto evita termos de lidar com a \n ao comparar, e garante que o fim do heredoc acontece só quando a linha é exatamente o LIMITER.

					Portanto entramos no if (remove_newline(line, limiter)) se o utilizador escreveu o LIMITER numa linha sozinha.
					Se a line == LIMITER, libertamos o buffer alocado pela função get_next_line() e saímos do while (1), não escrevendo essa linha no
					write end do pipe. Isto é crucial, pois o delimitador é tratado no shell como marca do fim e não faz parte dos dados enviados do heredoc.
					Logo a seguir ao sair do while (1), fechamos o write end - close(pipefd[1]). AO fecharmos o pipefd[1], o processo leitor (cmd1)
					recebe o EOF quando consumir (ler) tudo o que estava no buffer do pipe.
					Terminamos o processo produtor do heredoc, com exit(0).
					
					Caso contrário, não entramos nesse if (pois line != LIMITER e a linha faz parte do conteúdo do heredoc) e escrevemos a linha no write end do pipe.
					
					write(pipefd[1], line, ft_strlen(line));

					Enviamos para o write end do pipe o conteúdo da line.
					Como na função remove_newline() retiramos a newline '\n' (caso exista) do fim, o conteúdo da line só contém os caracteres visiveis sem a newline.
					Este write escreve no buffer do pipe. O processo do cmd1 irá ler isso depois pelo read end.
					
					write(pipefd[1], "\n", 1);

					De seguida, escrevemos explicitamente a newline '\n'.
					
					Porquê? Porque removemos a newline '\n' para poder comparar a linha exatamente com o delimiter.
							Depois da comparação, voltamos a garantir que “cada linha entregue ao consumidor termina com uma newline”, o que é o comportamento que os 
							comandos (e o shell) normalmente esperam.
							
					Assim, garantimos que cada linha escrita no terminal termina com uma newline \n: - Escrevemos a string (sem a newline \n).
																									- Depois escrevemos a newline \n.
																										
					Assim, o consumidor (cmd1) vai ler exatamente a linha + a newline \n.
					
					free(line);

					Libertamos a memória que get_next_line() alocou para a line.

					
					> Fechar o write end do pipe no processo child - close(pipefd[1]) e sinalizar o EOF

					Após sair do while (1) (seja por ter encontrado o LIMITER ou por ter atingido o EOF), fechamos o write end do pipe, para sinalizar o EOF ao processo
					leitor (cmd1).
					O processo leitor (cmd1) vai ler do read end do pipe (pipefd[0]).
					Enquanto existir pelo menos um write end aberto para o mesmo pipe, o read() do cmd1  pode bloquear e ficar à espera de mais dados e, consequentemente, 
					o read() não devolve 0 (que representa o EOF).
					Assim, quando todas as pontas de escrita (write ends) de um pipe são fechadas, o próximo read() do processo leitor devolve 0 (EOF) quando já leu tudo o
					que estava no buffer do pipe.

					Se não fechassemos o write end do pipe, o processo leitor (cmd1) poderia ficar bloqueado para sempre à espera de mais dados, pois o write end
					continuaria aberto, mesmo que o produtor do heredoc já tivesse terminado. O pipeline acabaria por não terminar nunca (ou só acabaria quando o cdm1
					terminasse e o kernel fechasse todos os FDs abertos).

					Depois de fecharmos o write end do pipe (e consequentemente depois de sinalizarmos o EOF ao leitor), terminamos o processo produtor do heredoc com exit(0),
					libertando quaisquer FDs ainda abertos (o kernel encerra tudo) e entrega um status 0 ao waitpid() no parent.
					O child fechou as pontas do pipe dele e terminou.
					Os FDs que estão abertos agora são os do parent (o primeiro comando da pipeline).


																	(teclado / STDIN)
																			│
																	get_next_line()
																			│          
																		"HELLO\n"
																			|
																	remove_newline()  (remove a '\n' se houver)
																			|
																		"HELLO"        ← agora sem '\n'
																			│
														┌─────────────────┴─────────────────┐
														│ write(pipefd[1], "HELLO", 5)      │  → vai para o buffer do pipe
														│ write(pipefd[1], "\n", 1)         │  → repõe newline para o leitor
														└─────────────────┬─────────────────┘
																			│
																	[ buffer do pipe ]
																			│
															read(pipefd[0])  ← cmd1 (primeiro comando)

							
															
					3) Em paralelo, no parent process (após o child process do heredoc):

						static pid_t    ft_heredoc(char **av, int *hdoc_readfd)
						{
								int pipefd[2];
								pid_t pid;

								get_pipe_and_fork(pipefd, &pid);
								if (pid == 0)
									read_till_limiter(av, pipefd);
								close(pipefd[1]);
								*hdoc_readfd = pipefd[0];
								return (pid);
						} 
						
						No parent process (pid > 0), fechamos o write end do pipe do parent - close(pipefd[1]) - pois o parent não vai escrever  no write end do pipe
						(só o child do heredoc é que escreve no wirte end do pipe).
						Isto é fundamental, pois se o parent não fechar o seu pipefd[1], o pipe continua a ter um writer ativo → o processo leitor (cmd1)
						não recebe EOF e pode ficar bloqueado.
						Ao fecharmos o write end do pipe no parent, garantimos que quando o child produtor do heredoc fechar o write end (pipefd[1]), o processo leitor 
						(cmd1) vê o EOF quando consumir (ler) tudo o que estiver no buffer do pipe.
						
						Como vimos, o child do heredoc escreve as linhas no write end do pipe (pipefd[1]) e depois de terminar, fecha o write end (pipefd[1]) e sai.
						No parent process, ele mantém o read end do pipe aberto, pois precisa dele para encadear o pipeline (passar o STDIN ao cmd1), ou seja,
						o parent process precisa de continuar com o "fio" do read end do pipe (pipefd[0]) para levar os dados do heredoc até ao próximo comando da pipeline.
						
						Assim, o parent guarda o número do FD do read end do pipe (pipefd[0]) no ponteiro hdoc_readfd.
						Com consequência, quem chamar a função ft_heredoc(av, &input_fd), o input_fd (que é *hdoc_readfd) passa a conter o número do FD do read end do pipe que será
						ligado ao STDIN do próximo comando da pipeline (cmd1) -> esse FD será usado como STDIN do próximo comando na pipeline (o cmd1).
						O read end (pipefd[0]) será utilizado como STDIN do primeiro comando da pipeline - *hdoc_readfd = pipefd[0].
						
						Se o parent não guardasse o read end (pipefd[0]), perderia o acesso ao conteúdo do heredoc e, assim, o próximo comando não teria uma referência de onde ler e de
						como saber qual é o FD do read end do pipe que deve usar como STDIN do cmd1, pelo o que o pipeline não funcionaria (o cmd1 ia ler de um STDIN vazio/errado).

						Por fim, o parent devolve o PID do child produtor do heredoc para que o chamador possa fazer waitpid(pid, ...) quando quiser.

						> Por que devolver esse PID?

							Para podermos fazer o waitpid(hdoc_pid, ...) no momento certo e evitar zombies.
							Se esperassemos pelo heredoc antes de criarmos o primeiro comando que vai ler do pipe, poderiamos provocar um bloqueio (o writer enche o pipe e fica bloqueado
							à espera do leitor que ainda não existe).

							No chamador, fazemos waitpid(hdoc_pid, NULL, 0) quando já criamos o primeiro comando da pipeline que vai ler do pipe.
							Ao fazer waitpid(hdoc_pid, NULL, 0), esperamos que o processo do heredoc termine e evitamos que fique como zombie.
							Não nos interessa o status do heredoc, desde que seja 0 (exit(0)), por isso passamos NULL no segundo argumento do waitpid().
							O waitpid(hdoc_pid, NULL, 0) bloqueia o chamador até o processo do heredoc terminar.

							O padrão correto: - Criar o heredoc (receber hdoc_pid + hdoc_readfd).
											- Criar o primeiro comando do pipeline e ligar o STDIN dele ao hdoc_readfd (via dup2).
											- Fazer waitpid(hdoc_pid, NULL, 0) — o leitor já existe, não há deadlock.

							Para não misturar o status final “oficial” do pipeline: - O child do heredoc não é um comando da pipeline, ele só produz dados.
																					- O status final do pipeline é o do último comando (cmdN).
																					- O status do heredoc não interessa, desde que seja 0 (exit(0)).
																					- Assim, fazemos waitpid(hdoc_pid, NULL, 0) para evitar zombies, mas não guardamos o status do heredoc.
																					- O waitpid() do último comando da pipeline é que devolve o status final do pipeline.
										
																					
						> Pipeline do Heredoc
						
						pid_t ft_heredoc_pipeline(int ac, char **av, char **envp)
						{
							pid_t last_pid;
							pid_t hdoc_pid;
							int input_fd;
							int i;

							if (ac < 6)      // heredoc + LIMITER + >= 2 cmds + outfile
								show_usage_exit2();
							input_fd = -1;
							hdoc_pid = ft_heredoc(av, &input_fd);
							i = 3;
							last_pid = -1;
							if (i < ac - 2)       // se há pelo menos um middle antes do last
							{
								last_pid = middle_child(av, &input_fd, envp, i++);
								waitpid(hdoc_pid, NULL, 0);
								while (i < ac - 2)
									last_pid = middle_child(av, &input_fd, envp, i++);
								last_pid = exec_last_and_append(ac, av, input_fd, envp);
							}
							else   // se não houver middles, vai direto para o último
								last_pid = exec_last_and_append(ac, av, input_fd, envp);
							return (last_pid);
						}

						> Explicação da pipeline do heredoc

						1) Verificamos se há argumentos suficientes (ac < 6) para o modo heredoc (heredoc + LIMITER + >= 2 cmds + outfile).
							Se não houver, mostramos a mensagem de uso e saímos com erro.

						2) Inicializamos input_fd a -1. Este será o FD que vai ligar o STDIN do primeiro comando da pipeline ao read end do pipe do heredoc.

						3) Chamamos ft_heredoc(av, &input_fd), que cria o pipe e o processo produtor do heredoc.
							O input_fd (que é *hdoc_readfd) passa a conter o número do FD do read end do pipe que será ligado ao STDIN do próximo comando da pipeline (cmd1).
							O hdoc_pid recebe o PID do processo produtor do heredoc, para podermos fazer o waitpid(hdoc_pid, NULL, 0) mais tarde.

						4) A linha de comandos é: ./pipex_bonus here_doc LIMITER cmd1  cmd2  ...  cmdN     outfile
														av[0] 	av[1]   av[2]  av[3] av[4] ... av[ac-2]  av[ac-1]

							O primeiro comando real é o cmd1, que é o av[3].
							O último comando é o cmdN, que é o av[ac - 2].
							O outfile é o av[ac - 1].

							Aqui, não há um um first_child() no heredoc - O STDIN inicial do primeiro comando (cmd1) vem do pipe do heredoc (input_fd) - que
							é o read end do pipe do heredoc.
							Assim, não há first_child() - o primeiro comando (cmd1) é criado diretamente com a função middle_child(), que liga o input_fd
							(read end do pipe do heredoc) ao STDIN do cmd1.
							POr isso, todos os comandos antes do último são tratados pela função middle_child() (incluindo o primeiro comando após o heredoc). 
						
							Assim inicializamos i com 3 (que define o indice do primeiro comando após o heredoc e o LIMITER) e last_pid a -1 (pois ainda não há comandos criados).

						5) Verificamos se há pelo menos um middle command antes do último comando (i < ac - 2).
							Se houver, então existe pelo menos um comando antes do último e entramos no if:

							a) last_pid = middle_child(av, &input_fd, envp, i++);
							
								Executamos o primeiro comando da pipeline (cmd1) após o heredoc com o middle_child().
								O input_fd (que é o read end do pipe do heredoc) é passado por referência para que o middle_child possa atualizar o input_fd
								para o read end do pipe criado para este comando.

								A função middle_child():

								pid_t	middle_child(char **av, int *prev_readfd, char **envp, int i)
								{
									int in_fd;
									int pipefd[2];
									pid_t pid;

									in_fd = *prev_readfd;                    // herda o input_fd do heredoc (read end do pipe do heredoc) ou do comando (middle) anterior
									get_pipe_and_fork(pipefd, &pid);         // cria um novo pipe (para ligar este comando ao proximo) e cria um novo child process (fork)
									if (pid == 0)
									{
										close(pipefd[0]);                     // fechamos o read end do child, pois ele não vai ler do pipe
										safe_dup2(in_fd, STDIN_FILENO);       // liga o input_fd (read end do pipe do heredoc ou do middle anterior) ao STDIN do child
										safe_dup2(pipefd[1], STDOUT_FILENO);  // liga o write end do novo pipe (para o próximo comando) ao STDOUT do child
										close(pipefd[1]);                     // fechamos a referencia ao write end do child, pois já está ligado ao STDOUT (já está redirecionado).
										close(in_fd);                         // fechamos a referencia ao input_fd do child, pois já está ligado ao STDIN (já está redirecionado).
										ft_exec_cmd(av[i], envp);
									}
									close(in_fd);                             // PARENT: fecha a referencia ao input_fd do parent, pois já não é necessário (já foi passado ao child)
									close(pipefd[1]);                         // PARENT: fecha a referencia ao write end do parent, pois o parent não vai escrever no pipe (só o child escreve)
									*prev_readfd = pipefd[0];                 // PARENT: atualiza o input_fd (read end do pipe do middle) para o próximo comando
									return (pid);                             // PARENT: devolve o PID do child criado
								}

								a2) Na função middle_child(), o parent processo cria um pipe e um processo filho (fork).
									Este novo pipe servirá para transportar o STDOUT deste comando para o STDIN do próximo.
									O pipe faz isso ao ligar o STDOUT deste comando (cmd1) ao STDIN do próximo comando (cmd2).

									get_pipe_and_fork(pipefd, &pid);
								
									Chamamos a função get_pipe_and_fork() para criar o pipe e o criar o primeiro processo a seguir ao heredoc (o primeiro comando).

									O kernel cria um objeto pipe com um buffer e devolve dois FDs abertos que apontam para as duas pontas do mesmo pipe:
						
									- pipefd[0]: read end -> aberto só para leitura do pipe: associado ao FD de leitura;
									- pipefd[1]: write end -> aberto só para escrita no pipe: associado ao FD de escrita;
				
									O fork() cria um novo processo (child consumidor do heredoc) que é uma cópia do parent.
									O child herda a mesma tabela de FDs do parent, ou seja, o child também tem o pipefd[0] e o pipefd[1] abertos.

									Depois do fork(), tanto o child como o parent têm as duas pontas do pipe abertas:

									Parent (após o fork):  FD 3 -> pipefd[0]
														   FD 4 -> pipefd[1]

									Child (cópia):  FD 3 -> pipefd[0]
													FD 4 -> pipefd[1]
						
									O fork() devolve o PID do child no parent (pid > 0) e devolve 0 no pid do child (pid == 0).

									Por que criar o pipe antes do fork?
									
									Para que tanto o parent quanto o child possuam os FDs do pipe e cada um feche o que não usa. 
									Isso é essencial para a correta sinalização do EOF e para evitar deadlocks.

									Este novo pipe criado servirá para transportar o STDOUT deste comando (cmd1) para o STDIN do próximo (cmd2).
									O pipe faz isso ao ligar o STDOUT deste comando (cmd1) ao STDIN do próximo comando (cmd2).

								a3) in_fd = *prev_readfd;
				
									O in_fd recebe o valor atual do *prev_readfd, que é o hdoc_fd passado por referência - o input que veio do ft_heredoc().
				    				O in_fd é o read end do pipe do heredoc - o pipe no qual o ft_heredoc() escreveu.
									No primeiro middle_child() (cmd1), o in_fd é o read end do pipe do heredoc (input_fd).

								a4) Se pid == 0, entramos no child consumidor do heredoc (que vai ser o nosso cmd1).
									Começamos por fechar o read end do pipe (pipefd[0]) neste processo, pois este processo não vai ler do read end do pipe, só 
									vai escrever no write end do pipe (o processo que vai ler (o leitor) vai ser o comando seguinte a este).

									-> Fechar o read end (pipefd[0]) no child consumidor do heredoc faz com que o número de pontas de leitura abertas (pipefd[0]) seja a correta,
									   assim, quando o child seguinte morrer, esse número será 0 e write no child consumidor do heredoc já não bloqueia.
									
									-> Ao fechar o read end, garantimos que o contador de referências do read end do pipe está correto.

								a5) safe_dup2(in_fd, STDIN_FILENO)

									Se o in_fd for um FD válido, o kernel fecha o STDIN (FD 0) (se estiver aberto) e faz o FD 0 apontar para o mesmo FD do in_fd.
									Por exemplo, se o FD do in_fd é 3, com o dup2(), o FD 0 passa apontar para o FD 3, que é o FD do in_fd.

									No child consumidor do heredoc, vamos ligar/redirecionar o STDIN (FD 0) ao FD do in_fd, que é o read end do pipe do heredoc.
									Assim, o STDIN do child consumidor do heredoc que antes apontava e lia do terminal, passa a ler do in_fd (o read end do pipe do heredoc).

									Resultado: o stdin passa a ler do read end do pipe do heredoc, pois o stdin (FD 0) aponta agora para o FD do in_fd (FD 0 -> in_fd)

									Porquê que fechamos ?

									Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
									O FD in_fd já não será utilizado.
									Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
									Se não fecharmos o read end do pipe do comando anterior e o processo sair por erro mais tarde, durante esse intervalo o kernel
									continua a ver que ainda há um leitor vivo.
									O processo escritor (o child consumidor do heredoc) pode não receber o SIGPIPE quando deveria, causando um bloqueio (ele fica à espera eternamente).
									Ao fechar imediatamente o in_fd no erro, o número de referencias ao read end do pipe é decrementado e assim , o processo escritor pode ver o 
									SIGPIPE no timming certo (quando os restantes leitores fecharem).
					
								a6) safe_dup2(pipefd[1], STDOUT_FILENO)

									O kernel fecha o STDOUT (FD 1) (se estiver aberto) e faz o STDOUT (FD 1) apontar para o mesmo FD que o write end do pipe.
									Por exemplo, se o FD do write end for 5, com o dup2(), o FD 1 passa a apontar para o FD 5, que é o FD do write end do pipe.

									No segundo child, vamos ligar/redirecionar o STDOUT (FD 1) ao FD do write end do pipe (pipefd[1]).
									Assim, o STDOUT do child consumidor do heredoc que antes apontava e escrevia no terminal, passa a escrever no write end do pipe.

									Resultado: o stdout passa a escrever no pipe, pois o stdout (FD 1) aponta agora para o FD do write end do pipe (FD 1 -> pipefd[1]).

									Porquê que fechamos ?

									Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
									O write end pipefd[1] já não será utilizado.
									Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
									Se não fecharmos o write end do pipe e o processo sair por erro mais tarde, durante esse intervalo o kernel
									continua a ver que ainda há um escritor vivo.
									O leitor (o child 2) pode não receber o EOF quando deveria, causando um bloqueio (ele fica à espera eternamente).
									Ao fechar imediatamente o pipefd[1] no erro, o número de referencias ao write end do pipe é decrementado e assim, o 
									leitor (o child 2) pode ver o EOF no timming certo (quando os restantes escritores fecharem).


								> Comparação do antes e depois do dup2():
									
									Antes:                                               		Depois:

									FD 0 (STDIN)  -> aponta para o terminal              		FD 0 (STDIN)  -> aponta para o in_fd (O stdin agora lê do in_fd, ou seja do read end do heredoc)
									FD 1 (STDOUT) -> aponta para o terminal              		FD 1 (STDOUT) -> aponta para o pipefd[1] (O stdout agora escreve no pipe)
									FD 2 (STDERR) -> aponta para o terminal              		FD 2 (STDERR) -> aponta para o terminal
									FD 3 -> aponta para o in_fd (read end do heredoc)           FD 3 -> aponta para o in_fd (original ainda continua aberto)
									FD 4 -> aponta para o pipefd[0] [FECHADO]            		FD 4 -> aponta para o pipefd[0] [FECHADO]
									FD 5 -> aponta para o pipefd[1] (write end)          		FD 5 -> aponta para o pipefd[1] (original ainda aberto)

									O in_fd e o pipefd[1] estão agora ligados aos FDs padrão 0 e 1.
									Como há essas novas referências para o STDIN e para o STDOUT, podemos fechar as referências originais (in_fd e pipefd[1]), pois o child process já não 
									precisa dessas referencias originais (pois já estão ligadas ao STDIN e ao STDIN).
									
									O in_fd e o pipefd[1] originais ainda estão abertos, mas já não são necessários.

								a7) close(pipefd[1]); 
									close(in_fd);

									A seguir ao dup2() fechamos o write end do pipe (pipefd[1]) e o in_fd, porque os FDs originais já não serão precisos.

									Como vimos, depois do dup2() ficámos com duas referências para cada destino:

									FD 0 (stdin) ─┐                          FD 1 (stdout) ─┐
												  └─► [in_fd]                               └─► [pipefd[1] do pipe]
										in_fd    ─┘                              pipefd[1] ─┘

								
									Fechamos os FDs originais, pois para o child seguinte ver o EOF, o kernel precisa que todas as pontas de leitura do pipe 
									estejam fechadas. Caso ficasse o pipefd[1] aberto para além do stdout (FD 1), haveria mais uma referencia de escrita,
									o que atrasaria o EOF do child seguinte (ele só vê o EOF quando todas as referencias de escrita fecharem). Fechando o pipefd[1],
									ficamos com apenas o stdout (FD 1) como escritor, assim, quando o processo terminar, o EOF chega ao child seguinte.

									Ao fechar: - evitamos leaks.
											   - reduzimos os contadores do pipe (útil para EOF no leitor),

									A tabela de FDs final ficará:   FD 0 (STDOUT) -> aponta para o in_fd (o read end do pipe do heredoc)
																	FD 1 (STDIN)  -> aponta para o pipefd[1]

								a8) Finalmente, chamamos a função ft_exec_cmd() para substituir o child consumidor do heredoc pelo comando av[i] (que neste primeiro caso será o av[3]).
									A partir daqui, o processo torna-se o binário do comando e usa os FDs redirecionados (STDIN e STDOUT ligados aos pipes).
									O cmd1 vai ler do in_fd, ou seja, do read end do pipe do heredoc, pois o stdin está ligado ao in_fd, e vai escrever no write end do pipe (pois o
									stdout está ligado ao write end do pipe).
									
									Se a execução do comando falhar, a função ft_exec_cmd() termina o processo com error_exit().

								a9) close(in_fd);
									close(pipefd[1]);
								
									No parent process (pid > 0), fechamos a referencia ao in_fd (que é o read end do pipe do heredoc), pois o parent já não precisa dele.
									Essa referencia já foi passada ao child.
									
									Fechamos, também, o write end do pipe (pipefd[1]), pois o parent não vai escrever no pipe - só o child process (do comando atual) escreve no pipe.
									Se não fechassemos essa referência, o kernel entenderia que ainda havia um writer vivo/aberto e assim, quando o leitor (o próximo comando da cadeia) tentasse
									ler do pipe, nunca veria o EOF, pois o kernel só envia o EOF quando todas as referencias ao write end do pipe estão fechadas.
									Isto causa bloqueio eterno no processo leitor, mesmo que o parent e o child consumir do heredoc já tenham morrido/terminado, pois ainda haveria um 
									writer aberto (o pipefd[1] do parent).
							
									Assim, fechar o pipefd[1] no parent: - evita deadlocks/bloqueios;
																		 - evita leak de FDs;
																		 - garante que o kernel consiga gerir corretamente o EOF para o proximo comando assim que o child 1 terminar (o kernel só envia o EOF
																		   quando todas as referencias ao write end do pipe estão fechadas).

								a10) *prev_readfd = pipefd[0];
						
									O parent process não fecha o read end do pipe (pipefd[0]), pois este é necessário para o próximo comando (outro middle_child() ou o exec_last_and_append()).
									O pipe no qual o middle_child() escreveu será a entrada (input_fd) do próximo comando.
									Para passar essa "entrada anterior" para o próximo comando, o parent guarda o FD do read end do pipe (pipefd[0]) na variável prev_readfd.
									A variável prev_readfd representa "o read end da etapa anterior" (o input_fd que vem de trás).
									
									Assim, atualizamos o prev_readfd (input_fd) para ser o read end do novo pipe (pipefd[0]), que será usado como input_fd (STDIN) do próximo comando.
									O prev_readfd é passado por referência para que na main() possamos atualizar o input_fd para o passarmos para o comando seguinte a ser executado.
									Assim, o próximo comando vai ler deste pipefd[0].

									Na pipeline, o parent process vai repetir este padrão: mantém o "fio" do read end do pipe (pipefd[0]) para o próximo comando, passando-o por referência, e assim, cada
									middle deixa preparado o read end para ser o in_fd seguinte.

								a11) Finalmente, o parent process devolve o PID do child process criado (do comando atual), para que o chamador possa fazer wait().

									 return (pid);
								



							b) Fazemos waitpid(hdoc_pid, NULL, 0) para recolher o PID do heredoc e evitar que fique num estado zombie.

								Na funçaõ waitpid():   pid_t waitpid(pid_t pid, int *status, int options);
								
								O PID é o do processo que queremos esperar que termine para recolhê-lo (no caso do heredoc é o hdoc_pid).
								O segundo argumento é o status do processo que terminou (se quisermos saber qual a causa do término do child, passamos um ponteiro para int, se não quisermos saber, passamos NULL).
								O terceiro argumento são as opções (0 = bloqueia até o processo terminar).

								> Porquê que recolhemos o PID (reap) ?

								QUando um child termina, ele não desaparece imediantamente, fica num estado "zombie" até que o parent faça wait() ou waitpid() para recolher o PID e o status do child.
								
								O processo writer do heredoc termina cedo (assim que vê o LIMITER ou o EOF). Se não recolhermos o PID dele, ele fica num estado zombie, até o parent fazer wait().
								Pois se o parent não fizer wait() ou waitpid(), o child fica como zombie, ocupando recursos do sistema desnecessariamente.
								Como este child do heredoc não participa no calculo final do exit status (pois o status final do pipeline é o do último comando), não faz sentido deixá-lo como zombie até ao final.
								Assim, é seguro colhê-lo agora: o waitpid liberta os recursos do sistema associados ao processo child terminado (remeove o estado zombie) e, opcionalmente, entrega o status ao parent.

								Até aqui, o middle_child() criou o primeiro comando (o primeiro consumidor do heredoc) com STDIN ligado ao read end do pipe do heredoc (input_fd) e devolve o PID desse comando.
								EM seguida, o waitpid(hdoc_pid, NULL, 0) espera especificamente que o processo writer do heredoc (o processo que estava a ler do teclado/STDIN e a escrever no write end do pipe)
								termine.
								Não nos interessa o status do heredoc, desde que seja 0 (exit(0)), por isso passamos NULL no segundo argumento.
								
								Como agora já existe um processo leitor (o primeiro middle), então não há risco de deadlock, pois o cmd1 vai ler do read end do pipe e o processo writer do heredoc pode continuar a 
								escrever sem ficar preso e terminar normalmente.
								Depois disso, já podemos esperar pelo processo writer com segurança: ele vai terminar assim que encontrar o LIMITER (ou o EOF), e não vai bloquear por falta de um processo leitor.
								
								Se trocassemos a ordem (esperar pelo processo writer antes de criar o processo leitor), poderiamos ficar bloqueados.

								EM suma, neste ponto, o processo leitor do heredoc (o cmd1) já existe (acabamos de criar o 1º comando que consome (lê) o 
								input_fd do heredoc).
								Agora é seguro colher o writer do heredoc: - evita deixar um zombie,
																		- e evita deadlock (se esperassemos pelo writer antes de criarmos o leitor, 
																			o writer podia bloquear com o pipe cheio).

																			
							c) Se houver mais comandos/processos antes do último, o while (i < ac - 2) vai encadear cada um deles com o middle_child(),
								atualizando o last_pid, a cada iteração, e passa o input_fd atualizado (read end do pipe do middle command anterior) para
								o próximo middle.
								Assim, cada middle lê do input_fd atual (read end do pipe do middle anterior), cria um novo pipe, e no final do parent process, o input_fd passa a ser o read end desse novo pipe,
								e assim vamos encadeando os processos até chegarmos ao último.

								while (i < ac - 2)
									last_pid = middle_child(av, &input_fd, envp, i++);
									
							
							d) Finalmente, criamos o último comando com a função exec_last_and_append(ac, av, input_fd, envp).
								O input_fd agora contém o read end do último criado.
								
								static pid_t exec_last_and_append(int ac, char **av, int oldfd, char **envp)
								{
									int append_fd;
									pid_t pid;

									pid = fork();                                                           // cria um novo processo para o último comando (av[ac - 2])
									if (pid < 0)
										error_exit("Fork Failed!");
									if (pid == 0)
									{
										append_fd = open(av[ac -1], O_WRONLY | O_CREAT | O_APPEND, 0644);
										if (append_fd == -1)
										{
											close(oldfd);
											error_exit("Error on outfile!");
										}
										safe_dup2(oldfd, STDIN_FILENO);
										safe_dup2(append_fd, STDOUT_FILENO);
										close(append_fd);
										close(oldfd);
										ft_exec_cmd(av[ac - 2], envp);
									}
									close(oldfd);
									return (pid);
								}
								
								oldfd: é o FD de leitura (read end) do pipe anterior, da pipeline.
									   No caso do heredoc, é o input_fd passado, que é o read end do pipe do middle anterior (ou seja, o read end do último middle criado).
									   Este oldfd será ligado ao STDIN do último comando (av[ac - 2]).

								append_fd: é o FD do outfile (que está no argumento av[ac - 1]) aberto em modo append.
										   Tudo o que o comando final escrever no STDOUT vai para o fim deste ficheiro.
								
								Na função exec_last_and_append():

								a1) O fork() cria um novo processo (o último child) que é uma cópia do parent.
									O child herda a mesma tabela de FDs do parent, ou seja, o último child também tem o pipefd[0] e o pipefd[1] abertos.

									Depois do fork(), tanto o child como o parent têm as duas pontas do pipe abertas:

									Parent (após o fork):  FD 3 -> pipefd[0]
														   FD 4 -> pipefd[1]

									Child (cópia):  FD 3 -> pipefd[0]
													  FD 4 -> pipefd[1]
						
									O fork() devolve o PID do child no parent (pid > 0) e devolve 0 no pid do child (pid == 0).
								
								
									Criamos um novo processo (fork) para o último comando.
									Se o fork falhar, terminamos com error_exit().

								a2)  Se pid == 0, entramos no último child (que vai ser o nosso last command).
									Começamos por abrir o ficheiro de output - o nosso outfile - (av[ac - 1]) em modo append (O_APPEND), ou seja, se o ficheiro
									já existir, o kernel move automaticamente a posição de escrita para o fim do ficheiro e escreve lá, ou seja, ele vai acrescentar
									no fim do ficheiro o conteúdo sem apagar ou sobrescrever o que já lá estava antes.
								
									append_fd = open(av[ac -1], O_WRONLY | O_CREAT | O_APPEND, 0644);

									O_WRONLY: abre o ficheiro apenas para escrita. O último comando vai escrever o output nesse ficheiro.

									O_CREAT: se o ficheiro não existir, o kernel cria-o. É por isso que passamos também o modo 0644 (permissões) como terceiro argumento do open(),
											que será aplicado só no caso de criação do ficheiro.

									O_APPEND: abre em modo append — sempre que fizermos write(), o kernel move automaticamente a posição de escrita para o fim do ficheiro e escreve
											lá, ou seja, ele vai acrescentar no fim do ficheiro o conteúdo sem apagar ou sobrescrever o que já lá estava antes.
											Assim, o output do último comando é adicionado ao fim do ficheiro, sem sobrescrever o que já lá estava.
											Corresponde ao operador >> no shell.

									0644: são as permissões do ficheiro que se aplicam apenas no caso de ele ser criado (O_CREAT). 
										rw-r--r-- ->  O dono pode ler e escrever (6 = 4 + 2), o grupo e os outros podem apenas ler (4).


									Se a abertura do ficheiro falhar (por exemplo, por falta de permissões), o open() devolve -1 e fechamos o prev_readfd e terminamos com error_exit().

									A função open() abre um ficheiro e devolve um FD.
									O kernel vai percorrer o ficheiro (av[ac - 1]) e verifica, com o O_WRONLY, se esse ficheiro tem permissão só de escrita.
									Se tudo estiver OK, é devolvido um FD novo (um número inteiro >= 0) que será armazenado no outfile_fd.
									
									Se o open() falhar ao abrir o ficheiro outfile (-1) - por falta de permissão - o último child não irá conseguir ligar/redirecionar o STDIN ao FD do
									outfile, para assim escrever no ficheiro.
									Assim, fechamos o oldfd (o read end do pipe do middle anterior), pois se não fechassemos o oldfd aqui, o processo escritor (o middle anterior)
									poderia ficar bloqueado à espera de escrever mais dados no write end do pipe ou receber o SIGPIPE (pois haveria ainda um leitor aberto, o oldfd
									do último child), mesmo que o parent e o middle anterior já tivessem morrido.
									Saímos com error_exit() para terminar o processo com o código de erro apropriado.

									> Porquê que precisamos de abrir o outfile ?

									Porque o último comando da pipeline vai escrever o output nesse ficheiro e, assim, ele precisa de um destino para o STDOUT.
									O último comando (av[ac - 2]) escreve o output no STDOUT, mas como redirecionámos o STDOUT para o ficheiro (append_fd), o output vai para o ficheiro.
									Se não abrissemos o outfile, não teriamos um FD válido para ligar o STDOUT do último comando e o redirecionamento falharia.
									Assim, o open() devolve um FD válido (append_fd) que podemos usar para ligar o STDOUT do último comando, com o dup2().
									Se o ficheiro não existir, o kernel cria-o (O_CREAT).
									Se a abertura do ficheiro falhar (por exemplo, por falta de permissões), o open() devolve -1 e fechamos o oldfd (input_fd) e terminamos com error_exit().

								a3) safe_dup2(oldfd, STDIN_FILENO)

									Se o oldfd for um FD válido, o kernel fecha o STDIN (FD 0) (se estiver aberto) e faz o FD 0 apontar para o mesmo FD do oldfd.
									Por exemplo, se o FD do in_fd é 3, com o dup2(), o FD 0 passa apontar para o FD 3, que é o FD do oldfd.

									No último child, vamos ligar/redirecionar o STDIN (FD 0) ao FD do oldfd, que é o read end do pipe do comando anterior.
									Assim, o STDIN do último child que antes apontava e lia do terminal, passa a ler do oldfd (o read end do pipe do comando anterior).

									Resultado: o stdin passa a ler do read end do pipe do comando anterior, pois o stdin (FD 0) aponta agora para o FD do oldfd (FD 0 -> oldfd).
									Assim, a leitura do STDIN do último comando vem do read end do pipe do middle anterior.

									Porquê que fechamos ?

									Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
									O FD prev_readfd já não será utilizado.
									Ao fechá-lo, libertamos a posição da tabela de FDs do processo.
									Se não fecharmos o read end do pipe do comando anterior e o processo sair por erro mais tarde, durante esse intervalo o kernel
									continua a ver que ainda há um leitor vivo.
									O processo escritor pode não receber o SIGPIPE quando deveria, causando um bloqueio (ele fica à espera eternamente).
									Ao fechar imediatamente o in_fd no erro, o número de referencias ao read end do pipe é decrementado e assim , o
									processo escritor pode ver o SIGPIPE no timming certo (quando os restantes leitores fecharem).

								a4) safe_dup2(outfile, STDOUT_FILENO)

									O kernel fecha o STDOUT (FD 1) (se estiver aberto) e faz o STDOUT (FD 1) apontar para o mesmo FD que o append_fd.
									Por exemplo, se o FD do write end for 5, com o dup2(), o FD 1 passa a apontar para o FD 5, que é o FD do ficheiro outfile.

									No último child, vamos ligar/redirecionar o STDOUT (FD 1) ao FD do outfile.
									Assim, o STDOUT do ultimo child que antes apontava e escrevia no terminal, passa a escrever no ficheiro outfile.

									Resultado: o stdout passa a escrever no ficheiro outfile, pois o stdout (FD 1) aponta agora para o FD do append_fd..

									Porquê que fechamos ?

									Se o dup2() falhar, não vamos continuar a configurar o processo para o execve.
									O outfile já não será utilizado.
									Ao fechá-lo, libertamos a posição da tabela de FDs do processo.

									> Comparação do antes e depois do dup2():
									
									Antes:                                               		      Depois:

									FD 0 (STDIN)  -> aponta para o terminal              		      FD 0 (STDIN)  -> aponta para o oldfd (O stdin agora lê do oldfd, ou seja do read end)
									FD 1 (STDOUT) -> aponta para o terminal              		      FD 1 (STDOUT) -> aponta para o append_fd (O stdout agora escreve no append_fd)
									FD 2 (STDERR) -> aponta para o terminal              		      FD 2 (STDERR) -> aponta para o terminal
									FD 3 -> aponta para o oldfd (read end do comando anterior)        FD 3 -> aponta para o oldfd (original ainda continua aberto)
									FD 4 -> aponta para o append_fd           						  FD 4 -> aponta para o append_fd (original ainda aberto)

									O oldfd e o append_fd estão agora ligados aos FDs padrão 0 e 1.
									Como há essas novas referências para o STDIN e para o STDOUT, podemos fechar as referências originais (oldfd e append_fd), pois o child process já não 
									precisa dessas referencias originais (pois já estão ligadas ao STDIN e ao STDOUT).

								a5) close(oldfd);
									close(append_fd);

									Depois dos dois dup2(), o FD 0 (STDIN) está ligado ao oldfd (read end do pipe) e o FD 1 (STDOUT) está ligado ao ficheiro de output (append_fd).
									A seguir ao dup2() fechamos o oldfd (read end do pipe do middle anterior) e o append_fd, porque os FDs originais já não serão precisos.

									Como vimos, depois do dup2() ficámos com duas referências para cada destino:

									FD 0 (stdin) ─┐                          FD 1 (stdout) ─┐
												  └─► [oldfd]                               └─► [append_fd]
									       oldfd ─┘                              append_fd ─┘


									Fechamos os FDs originais, pois para o último child ver o EOF, o kernel precisa que todas as pontas de leitura do pipe
									estejam fechadas. Caso ficasse o oldfd aberto para além do stdout (FD 1), haveria mais uma referencia de escrita,
									o que atrasaria o EOF do último child (ele só vê o EOF quando todas as referencias de escrita fecharem). Fechando o oldfd,
									ficamos com apenas o stdout (FD 1) como escritor, assim, quando o processo terminar, o EOF chega ao último child.

									Ao fechar: - evitamos leaks.
											- reduzimos os contadores do pipe (útil para EOF no leitor),

									A tabela de FDs final ficará:   FD 0 (STDOUT) -> aponta para o oldfd (o read end do comando anterior)
																	FD 1 (STDIN)  -> aponta para o append_fd

							
								a6) Finalmente, chamamos a função ft_exec_cmd() para substituir o último child pelo comando av[ac - 2] (o último comando).
								   A partir daqui, o processo torna-se o binário do comando e usa os FDs redirecionados (STDIN e STDOUT ligados aos pipes e ao ficheiro).
								   O last command vai ler do prev_readfd, ou seja, do read end do pipe do comando anterior (cmdN-1), pois o stdin está ligado ao oldfd,
								   e vai escrever no outfile (pois o stdout está ligado ao append_fd).
									
								   Se a execução do comando falhar, a função ft_exec_cmd() termina o processo com error_exit().

								a7) No parent process (pid > 0), fechamos a referência ao oldfd (read end do pipe do comando anterior), pois o parent já não precisa dele.
									Essa referência já foi passada ao child process (do comando atual) .
									Se não fechassemos essa referência, haveria um leak de FDs e o kernel não conseguiria gerir corretamente o EOF para o proximo comando.

									close(oldfd);

								a8) Finalmente, o parent process devolve o PID do child process criado (do comando atual), para que o chamador possa fazer wait().

									return (pid);

								

							    > Finalmente, na main(), fazemos wait_processes(last_pid) para esperar que todos os processos filhos terminem e recolher o exit status do último comando.
                                
								int	wait_processes(int last_pid)
								{
									int	pid;
									int	status;
									int	last_status;

									pid = 1;
									last_status = 0;
									while (pid > 0)
									{
										pid = wait(&status);
										if (pid == last_pid && WIFEXITED(status))
											last_status = WEXITSTATUS(status);
									}
									return (last_status);
								}

								AO longo da pipeline, fizemos fork() várias vezes para criar vários processos filhos (um filho por cada comando da pipeline).
								Cada processo filho, depois de fazer o o redirecionamento dos FDs, chamou execve() para substituir o processo pelo binário do comando.
								Assim, no final, temos vários processos filhos a correr em paralelo (um por cada comando da pipeline).
								
								O parent process (o processo main) precisa de esperar que todos os processos filhos terminem a sua funçao para recolher todos eles 
								para evitar zombies.
								Além disso, o parent process precisa de saber o exit status do último comando da pipeline, para ser esse o exit status que ele vai
								devolver no final do programa pipex.
								É por isso que recebemos o last_pid do último comando criado (o PID do último child) e o passamos para a função wait_processes(), para
								que ela saiba qual o PID do último comando.

								> pid: é o PID do processo que terminou (devolvido pelo wait()).
								> status: é o status do processo que codifica como o filho terminou (devolvido pelo wait()).
								> last_status: é o exit code do último comando (last_pid) que vamos devolver no final do programa pipex.
								
								a) Inicializamos pid a 1 (um valor > 0) para entrar no while.
								   Inicializamos last_status a 0 (valor default, caso, por algum motivo o último não terminar normalmente, mantém-se 0).

								b) while (pid > 0)

									Repetimos o loop enquanto o pid for > 0, ou seja, enquanto houver processos filhos para esperar.
									Quando não houver mais processos filhos, o wait() devolve -1 e saímos do loop.
									No loop, fazemos:

									> pid = wait(&status);

										A função wait() espera que qualquer processo filho termine e devolve o PID desse processo.
										O argumento status é um ponteiro para int onde o wait() vai guardar o status do processo que terminou.
										Esse status codifica como o processo terminou - se saiu por exit (terminação normal) ou se morreu por sinal, etc.
										
										O wait(&status) devolve: - um número maior do que 0 (> 0), que corresponde ao PID de um filho que acabou, pelo o que continuamos a colher.
																 - -1, pois não há mais filhos, o que faz com que o loop termine (porque pid > 0 deixa de ser verdade).
											
										Assim, o wait(&status) bloqueia (fica à espera) até que um filho termine, e quando isso acontece, devolve o PID do filho que terminou e 
										guarda o status desse filho na variável status.
										
									> if (pid == last_pid && WIFEXITED(status))
									
									   Aqui verificamos se o PID do filho que terminou é igual ao last_pid (ou seja, se é igual ao PID do último comando da pipeline) e se esse
									   filho terminou normalmente (WIFEXITED(status)).
									   Em caso afirmativo, extraímos o exit code desse filho com WEXITSTATUS(status) e guardamos em last_status.
										
									   Assim, no final do loop, teremos o exit code do último comando da pipeline na variável last_status.
										
									   Se o filho não terminou normalmente (por exemplo, morreu por sinal), não atualizamos o last_status, que mantém o valor anterior (inicialmente 0).
									   Assim, se o último comando morrer por sinal, o pipex devolve 0 como exit code (poderíamos escolher outro valor se quiséssemos).


								    > Finalmente, devolvemos o last_status, que é o exit code do último comando da pipeline (ou 0 se o último comando não terminou normalmente).
								      Esse valor será o exit code do programa pipex.



								Fazemos waitpid(hdoc_pid, NULL, 0) para recolher o PID do heredoc e evitar que fique num estado zombie.

								Na funçaõ waitpid():   pid_t waitpid(pid_t pid, int *status, int options);
								
								O pid é o do processo que queremos esperar que termine para recolhê-lo (no caso do heredoc é o hdoc_pid).
								O segundo argumento é o status do processo que terminou (se quisermos saber qual a causa do término do child, passamos um ponteiro para int, se não quisermos saber, passamos NULL).
								O terceiro argumento são as opções (0 = bloqueia até o processo terminar).

								> Porquê que recolhemos o PID (reap) ?

								QUando um child termina, ele não desaparece imediantamente, fica num estado "zombie" até que o parent faça wait() ou waitpid() para recolher o PID e o status do child.
								
								O processo writer do heredoc termina cedo (assim que vê o LIMITER ou o EOF). Se não recolhermos o PID dele, ele fica num estado zombie, até o parent fazer wait().
								Pois se o parent não fizer wait() ou waitpid(), o child fica como zombie, ocupando recursos do sistema desnecessariamente.
								Como este child do heredoc não participa no calculo final do exit status (pois o status final do pipeline é o do último comando), não faz sentido deixá-lo como zombie até ao final.
								Assim, é seguro colhê-lo agora: o waitpid liberta os recursos do sistema associados ao processo child terminado (remeove o estado zombie) e, opcionalmente, entrega o status ao parent.

								Até aqui, o middle_child() criou o primeiro comando (o primeiro consumidor do heredoc) com STDIN ligado ao read end do pipe do heredoc (input_fd) e devolve o PID desse comando.
								EM seguida, o waitpid(hdoc_pid, NULL, 0) espera especificamente que o processo writer do heredoc (o processo que estava a ler do teclado/STDIN e a escrever no write end do pipe)
								termine.
								Não nos interessa o status do heredoc, desde que seja 0 (exit(0)), por isso passamos NULL no segundo argumento.
								
								Como agora já existe um processo leitor (o primeiro middle), então não há risco de deadlock, pois o cmd1 vai ler do read end do pipe e o processo writer do heredoc pode continuar a 
								escrever sem ficar preso e terminar normalmente.
								Depois disso, já podemos esperar pelo processo writer com segurança: ele vai terminar assim que encontrar o LIMITER (ou o EOF), e não vai bloquear por falta de um processo leitor.
								
								Se trocassemos a ordem (esperar pelo processo writer antes de criar o processo leitor), poderiamos ficar bloqueados.

								EM suma, neste ponto, o processo leitor do heredoc (o cmd1) já existe (acabamos de criar o 1º comando que consome (lê) o 
								input_fd do heredoc).
								Agora é seguro colher o writer do heredoc: - evita deixar um zombie,
																		- e evita deadlock (se esperassemos pelo writer antes de criarmos o leitor, 
																			o writer podia bloquear com o pipe cheio).