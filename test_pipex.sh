echo -e "This is the content for file1.\\nola\\nola" > a
touch b c d e f g h i j k l m n
echo testb - sort reverse
valgrind --leak-check=full ./pipex a "sort -r" " uniq -c" b
echo -e \\n
echo testc - Lines with file
valgrind --leak-check=full ./pipex a "grep file" "wc -w" c
echo -e \\n
echo testd - Counts files folder
valgrind --leak-check=full ./pipex a "ls -l" "wc -l" d
echo -e \\n
echo teste - Counts lines
valgrind --leak-check=full ./pipex a "wc -l" "cat" e
echo -e \\n
echo testf - Unique lines + sort
valgrind --leak-check=full ./pipex a "sort" "uniq -c" f
echo -e \\n
echo testg - Number bytes
valgrind --leak-check=full ./pipex a "cat" "wc -c" g
echo -e \\n
echo testh - Lines with error
valgrind --leak-check=full ./pipex a "grep 'error'" "wc -l" h
echo -e \\n
echo testi - Sort a
valgrind --leak-check=full ./pipex a "sort" "cat" i
echo -e \\n
echo testj - too few arguments
valgrind --leak-check=full ./pipex a "find -name .txt" "cat"
echo -e \\n
echo testk - too many arguments
valgrind --leak-check=full ./pipex a "find -name .txt" "cat" k "sort"
echo -e \\n
echo testl - empty strings/arguments
valgrind --leak-check=full ./pipex "" "" "" ""
echo -e \\n
echo testm - non existing file
valgrind --leak-check=full ./pipex z "cat" "wc -c" m
echo -e \\n
echo testn - invalid command
valgrind --leak-check=full ./pipex a "invalid command" "wc -c" m
echo -e \\n