#!/bin/bash

# argumenty wiersza polecen
PROG=$1
DIR=$2

# polecenia
COMMAND="./$PROG"
VALGRIND="valgrind --error-exitcode=123 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all"
VALGRIND_COMMAND="$VALGRIND ./$PROG"

# indeks danego testu
i=1

for FILE in $DIR/*.in
do
   OUTPUT="${FILE%.in}_output.out"
   OUTPUT_DIFF="${FILE%.in}_output.diff"
   ERROR="${FILE%.in}_error.err"
   ERROR_DIFF="${FILE%.in}_error.diff"

   # uruchomienie programu i zapisanie wyjscia i bledow do plikow
   $COMMAND < "$FILE" > "$OUTPUT" 2> "$ERROR"

   # porownanie z docelowymi wynikami i zapisanie roznic do plikow
   diff "${FILE%.in}.out" "$OUTPUT" > "$OUTPUT_DIFF"
   OUTPUT_RET=$?
   diff "${FILE%.in}.err" "$ERROR" > "$ERROR_DIFF"
   ERROR_RET=$?

   # valgrind
   $VALGRING_COMMAND <"$FILE"> /dev/null
   VALGRIND_RET=$?

   # sprawdzenie wyjscia
   if [ $OUTPUT_RET -eq 0 ]
   then
        echo "Test $i - $FILE: OUTPUT OK"
   else
        echo "Test $i - $FILE: OUTPUT FAIL"
        head -5 $OUTPUT_DIFF
   fi

   # sprawdzenie bledow
   if [ $ERROR_RET -eq 0 ]
   then
        echo "Test $i - $FILE: ERROR OK"
   else
        echo "Test $i - $FILE: ERROR FAIL"
        head -5 $ERROR_DIFF
   fi

   # sprawdzenie zarzadzania pamiecia
   if [ $VALGRIND_RET -eq 123 ]
   then
        echo "Test $i - $FILE: MEMORY FAIL"
   else
        echo "Test $i - $FILE: MEMORY OK"
   fi

   i=$((i+1))

   # usuniecie plikow
   rm "$OUTPUT"
   rm "$OUTPUT_DIFF"
   rm "$ERROR"
   rm "$ERROR_DIFF"

done
