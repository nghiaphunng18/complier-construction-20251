cd incompleted
make clean
make
cd ..
for i in {1..6}; do
echo "--- Check example$i ---"
    ./incompleted/kplc tests/example$i.kpl > tests/output$i.txt
    diff tests/output$i.txt tests/result$i.txt

    echo "Done example $i"
    echo ""

done
