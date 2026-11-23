Author: Filippov Artem Aleksandrovich

2.1 Process monitoring
2.1.1
echo "PID=$$ PPID=$PPID UID=$UID"
id
tty
ps -o pid,ppid,uid,user,tty,comm -p $$
ps -o pid,ppid,comm -p $PPID

2.1.2
ps -ef
ps aux
ps -eo pid,ppid,user,stat,cmd | grep -E 'systemd|bash'

2.1.3
pstree -p
pstree -ps $$

2.1.4
sleep 120 &
pgrep -a -u "$USER" sleep
ps -o pid,ppid,user,stat,cmd
echo $$

2.1.5
P=$(pgrep -n sleep)
top -p $P
ps -o pid,stat,%cpu,%mem,etime,time,cmd -p $P
kill $P

2.1.6
sleep 300 &
P=$!
grep -E '^(Name|State|Pid|PPid|Uid|Threads)' /proc/$P/status
od -c /proc/$P/cmdline

2.1.7
echo $$
readlink /proc/self
readlink /proc/self
ls -ld /proc/$$
cut -d' ' -f1,4 /proc/self/stat
grep -E '^(Name|Pid)' /proc/self/status

2.1.8
sleep 300 &
P=$!
ls -l /proc/$P/exe /proc/$P/cwd
ps -o pid,comm,args -p $P

2.1.9
ps -eo pid,nlwp,comm
ps -L -o pid,lwp,nlwp,comm -p 4241
grep Threads /proc/4241/status
ls /proc/4241/task

2.1.10
tail -f /dev/null &
P=$!
cat /proc/$P/maps
grep -E 'VmSize|VmRSS' /proc/$P/status

2.1.11
ps -o pid,ppid,user,stat,comm --ppid 2
ps -o pid,ppid,user,stat,comm -p $$
ls -l /proc/2/exe
cat /proc/2/cmdline | wc -c
wc -l < /proc/2/maps
ls -l /proc/$$/exe
cat /proc/$$/cmdline | wc -c
wc -l < /proc/$$/maps
sudo ls -l /proc/2/exe

2.2 Managment of tasks, signals and priorities
2.2.1
sleep 1000
Ctrl+Z
jobs -l
ps -o pid,stat,cmd -p 3614

2.2.2
bg %1
jobs
ps -o pid,stat -p 3614
fg %1
jobs
pgrep sleep

2.2.3
T1:
(while true; do echo "A $BASHPID"; sleep 1; done) >> test.log &
(while true; do echo "B $BASHPID"; sleep 1; done) >> test.log &
jobs -l
T2:
tail -f ~/BOS_26/lab4/test.log

2.2.4
kill -STOP %1
ps -o pid,stat,cmd -p 3526,3534
jobs 

2.2.5
kill -COUNT %1
kill -TERM %1
kill -TERM %2
jobs
ps -p 3526,3534

2.2.6
T2:
tty
sleep 1000
T1:
pkill -INT -t pts/1 sleep
T2:
echo $?
sleep 1000
Ctrl+C
echo $?

2.2.7
T1:
nano trap.sh
chmod +x trap.sh
./trap.sh
T2:
tail -f trap.log
T3:
kill -INT 4712
kill -TERM 4712
T1:
./trap.sh
T3:
kill -KILL 5050

2.2.8
taskset -c 0 nice -n 0 yes > /dev/null &
taskset -c 0 nice -n 10 yes > /dev/null &
ps -o pid,ni,pri,%cpu,comm -C yes
top -p 5215,5217

2.2.9
renice -n 15 -p 5217
renice -n 10 -p 5217
killall yes

2.2.10
bash
echo $$
exec ps -o pid,ppid,comm
echo $$

2.3 File descriptors and channels
2.3.1
ls -l /proc/$$/fd
ls -l /dev/stdin /dev/stdout /dev/stderr
readlink -f /dev/stdin

2.3.2
ls /etc/hostname /nonexist > out.txt 2> err.txt
cat out.txt
cat err.txt
ls /etc/hostname /nonexist > all.txt 2>&1
cat all.txt

2.3.3
exec 3> test.txt
exec 4< test.txt
echo hello >&3
echo world >&3
ls -l /proc/$$/fd
cat /proc/$$/fdinfo/3 /proc/$$/fdinfo/4
read -r l <&4
echo $l
cat /proc/$$/fdinfo/4
read -r l <&4
echo $l
grep pos /proc/$$/fdinfo/4

2.3.4
rm test.txt
cat test.txt
ls -l /proc/$$/fd
cat /proc/$$/fd/4
exec 3>&- 4<&-
ls /proc/$$/fd

2.3.5
T1:
cat | tr a-z A-Z
T2:
pgrep -a -x cat
pgrep -a -x tr
ls -l /proc/4019/fd /proc/4020/fd

2.3.6
mkfifo myfifo
ls -l myfifo
stat myfifo
file myfifo

2.3.7
T1:
cat myfifo
T2:
cat > myfifo
echo test > myfifo

2.3.8
cat myfifo > r1.txt &
cat myfifo > r2.txt &
seq 1 100000 > myinfo
wait
wc -l r1.txt r2.txt

2.3.9
chmod 600 myfifo
ls -l myfifo
sudo -u nobody sh -c 'echo x > '"$PWD"/myfifo

2.4 Process control software
2.4.1
T1 and T2:
make pinfo
./pinfo &
pid=$!
ps -o pid,ppid,ruid,euid,comm -p "$pid"
grep -E '^(Pid|PPid|Uid):' "/proc/$pid/status"

2.4.2
make forkdeme
./forkdemo

2.4.3
make forkdemo3
./forkdemo3
./forkdemo --signal

2.4.4
make launcher childinfo
./launcher
./launcher ./no-such-program

2.4.5
make signalwatch
./signalwatch &
pid=$!
kill -USR1 "$pid"
ps -p "$pid"
kill -TERM "$pid"
wait "$pid"

2.4.6
make pipe-demo
./pipe-demo

2.4.7
T1:
make fifo-reader fifo-writer
mkfifo -m 600 lab.fifo
ls -l lab.fifo
./fifo-reader lab.fifo
T2:
./fifo-writer lab.info

2.4.8
make shm-writer shm-reader
id=$(./shm-writer)
ipcs -m -i "$id"
./shm-reader "$id"
ipcs -m

2.4.9
make cowdemo
./cowdemo

2.4.10
T1:
make zombie
./zombie
T2:
ps -o pid,ppid,stat,comm -p 5021
T1:
./zombie --wait
T2:
ps -o pid,ppid,stat,comm -p 5033

2.4.11
make orphan
./orphan

2.4.12
make threads
./threads &
pid=$!
sleep 1
ls "/proc/$pid/task"
ps -L -p "$pid" -o pid,tid,comm
wait "$pid"

2.4.13
make forkdemo launcher childinfo myrun
strace -e trace=process -o forkdemo.trace ./forkdemo
strace -f -e trace=process -o forkdemo-f.trace ./forkdemo
strace -e trace=process -o launcher.trace ./launcher
strace -f -e trace=process -o launcher-f.trace ./launcher
strace -e trace=process -o myrun.trace ./myrun /bin/true
strace -f -e trace=process -o myrun-f.trace ./myrun /bin/true
cat forkdemo-f.trace
cat launcher-f.trace
cat myrun-f.trace

2.5 Implementation of system utilities
2.5.1
make myps
./myps
ps -U "$(id -u)" -o pid,ppid,state,comm

2.5.2
make myrun
./myrun /bin/true
./myrun /bin/false
./myrun no-such-lab4-command
./myrun ls -l pinfo.c

2.5.3
make mytimeout
./mytimeout 5 sleep 1
./mytimeout 1 sleep 10
./mytimeout 1 bash -c 'trap "" TERM; exec sleep 10'

2.5.4
make safe-launch
LAB_TEST=visible ./safe-launch childinfo 'hello; echo SECOND'
./safe-launch childinfo
./safe-launch date

2.5.5
make mypmap
./mypmap "$$"
./mypmap "$$" r-xp
pmap "$$"
./mymap 99999999

2.5.6
make launcher audit-target proc-audit
./launcher --audit &
pid=$!
./proc-audit "$pid"
ps -p "$pid" -o pid,comm,args
readlink "/proc/$pid/exe"
