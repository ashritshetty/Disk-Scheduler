echo "start"
date
(for i in `seq 1 $1` 
do
./randread /usr  123$i 0.9  0.7  > out.1 ; 
done
echo "1 done"
date ) &

(for i in `seq 1 $1` 
do
./randread /home 125$i 0.8  0.8  > out.2 
done
echo "2 done"
date ) &

(for i in `seq 1 $1` 
do
./randread /etc 778$i 0.9  0.8  > out.3 
done
echo "3 done"
date ) &

(for i in `seq 1 $1` 
do
./randread /bin     523$i 0.7  0.6  > out.4 
done
echo "4 done"
date ) &

(for i in `seq 1 $1` 
do
./randread /usr 135$i 0.9  0.8  > out.5 
done
echo "5 done"
date ) &

(for i in `seq 1 $1` 
do
./randread /home 623$i 0.9  0.8  > out.6 
done
echo "6 done"
date ) &

(for i in `seq 1 $1` 
do
./randread /etc  923$i 0.9  0.7  > out.7 
done
echo "7 done"
date ) &

(for i in `seq 1 $1` 
do
./randread /   823$i 0.6  0.6  > out.8 
done
echo "8 done"
date ) &

