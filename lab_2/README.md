- make clean
- make
- for i in {1..7}; do

  echo "--- Test $i ---"

  ./incompleted/parser test/example$i.kpl > test/output$i.txt

  diff test/output$i.txt test/result$i.txt

  echo ""

done
