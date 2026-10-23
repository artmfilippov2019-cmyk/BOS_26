Author: Filippov Artem Aleksandrovish

2.1 File access control
2.1.1
mkdir ~/test
cd ~/test
touch file
ls -l file

2.1.2
chmod 000 file
ls -l file
echo "hello" > file
cat file

2.1.3
chmod 200 file
echo "hello" > file
cat file

2.1.4
chmod g+r file
cat file

2.1.5
chmod u+r file
cat file

2.1.6
mkdir dir
echo "hello" > dir/new_file
ls -l dir

2.1.7
chmod a-x dir
ls dir
ls -l dir
cat dir/new_file
rm dir/new_file

2.1.8
chown root file

2.1.9
umask 007
touch file2
ls -l file2

2.1.10
umask 000
touch file3
mkdir dir3
ls -ld file3 dir3

2.1.11
su -
# chown root /home/artem/test/file
# chmod 400 /home/artem/test/file
# ls -l /home/artem/test/file
# exit

2.1.12
cat ~/test/file
su - 
# chmod 440 /home/artem/test/file
# exit
cat ~/test/file

2.2 User and group managment
2.2.1
whoami
id
groups

2.2.2
grep "^$USER:" /etc/passwd
getent group "$(id -gn)"

2.2.3
su -
# groupadd labgroup
# getent group labgroup

2.2.4
# useradd -m -s /bin/bash labuser
# getent passwd labuser
# id labuser

2.2.5
# passwd labuser
# exit
su - labuser
whoami
id
pwd
exit

2.2.6
su - labuser
touch ~/file
ls -l ~/labfile
exit

2.2.7
su -
# usermod -aG labgroup labuser
# id labuser
# su - labuser -c id

2.2.8
# useradd -m -s /bin/bash labuser2
# passwd labuser2
# usermod -aG labgroup labuser2
# getent group labgroup
# id labuser
# id labuser2 

2.2.9
# mkdir /tmp/lab_shared
# chgrp labgroup /tmp/lab_shared
# chmod 770 /tmp/lab_shared
# ls -ld /tmp/lab_shared
# su -labuser -c 'touch /tmp/lab_shared/test1'
# su -labuser2 -c 'touch /tmp/lab_shared/test2'
# ls /tmp/lab_shared

2.2.10
# su -labuser -c 'echo hello > /tmp/lab_shared/file_u1'
# ls -l /tmp/lab_shared/file_u1
# su - labuser2 -c 'cat /tmp/lab_shared/file_u1'
# su - labuser2 -c 'echo new >> /tmp/lab_shared/file_u1'
# su - labuser2 -c 'rm -f /tmp/lab_shared/file_u1'

2.2.11
# chmod g+s /tmp/lab_shared
# ls -ld /tmp/lab_shared
# su - labuser -c 'touch /tmp/lab_shared/u1'
# su - labuser2 -c 'touch /tmp/lab_shared/u2'
# ls -l /tmp/lab_shared

2.2.12
# mkdir /tmp/lab_sticky
# chmod 1777 /tmp/lab_sticky
# ls -ld /tmp/lab_sticky
# su - labuser -c 'touch /tmp/lab_sticky/u1'
# su - labuser2 -c 'touch /tmp/lab_sticky/u2'
# su - labuser2 -c 'rm -f /tmp/lab_sticky/u1'
# su - labuser -c 'rm -f /tmp/lab_sticky/u2'

2.2.13
# usermod -L labuser
# exit
su - labuser
su -
# usermod -U labuser
# exit
su - labuser

2.2.14
su -
# chage -l labuser
# chage -d 0 labuser
# exit
su - labuser

2.2.15
su -
# usermod -s /bin/sh labuser
# getent passwd labuser
# usermod -s /bin/bash labuser
# getent passwd labuser

2.2.16
# su - labuser2 -c 'echo hello > ~/file' 
# userdel labuser2
# ls -la /home/labuser2

2.2.17
# cat /etc/passwd
# getent passwd daemon bin www-data artem

2.2.18
# grep UID_MIN /etc/login.defs
# grep UID_MAX /etc/login.defs
# grep SYS_UID_MIN /etc/login.defs
# grep SYS_UID_MAX /etc/login.defs
# cut -d: -f1,3 /etc/passwd | sort -t: -k2 -n

2.2.19
# useradd -m -s /bin/bash testuser
# useradd -r testsystem
# getent passwd testuser testsystem
# id testuser
# id testsystem
# ls -ld /home/testuser /home/testsystem
# chage -l testuser
# chage -l testsystem

2.2.20
# userdel testsystem
# useradd -r -m testsystem
# ls -ld /home/testsystem

2.2.21
grep -E 'nologin|false' /ect/passwd
which nologin false
ls -l /usr/sbin/nologin /usr/bin/false
cat /etc/shells
man nologin
man false

2.2.22
su -
# usermod -s /usr/sbin/nologin testuser
# grep '^testuser:' /etc/passwd
# su - testuser
# runuser -u testuser -- id

2.2.23
# usermod -s /bin/false testuser
# grep '^testuser:' /etc/passwd
# su - testuser
# echo $?
# runuser -u testuser -- id

2.2.24
# usermod -s /bin/bash testuser
# grep '^testuser:' /etc/passwd
# su - testuser
whoami
echo $SHELL
exit

2.2.25
# usermod -s /usr/bin/date testuser
# grep '^testuser:' /etc/passwd
# su - testuser
# usermod -s /bin/bash testuser
# grep '^testuser:' /etc/passwd

2.2.26
# groupadd -r testsystemgroup
# groupadd testgroup
# getent group testsystemgroup testgroup

2.2.27
grep GID_MIN /etc/login.defs
grep GID_MAX /etc/login.defs

2.2.28
# useradd -r -g testsystemgroup -M -s /usr/sbin/nologin serviceuser
# grep '^serviceuser:' /etc/passwd
# id serviceuser
# ls -ld /home/serviceuser

2.2.29
# passwd testuser
su - testuser
sudo apt update
exit

2.2.30
# visudo -f /etc/sudoers.d/testuser
in file: testuser ALL=(root) /usr/bin/id
# sudo -l -U testuser
sudo id
sudo whoami
exit

2.2.31
# visodo -f /etc/sudoers.d/testuser
in file: testuser ALL=(root) /usr/bin/id, /usr/bin/cat /etc/shadow
su - testuser
sudo id
sudo cat /etc/shadow
sudo cat /etc/gshadow
sudo ls /root
exit

2.2.32
# rm /etc/sudoers.d/testuser
# groupadd testadmins
# visudo -f /etc/sudoers.d/testadmins
in file: %testadmins ALL=(root) /usr/bin/ls /root
# usermod -aG testadmins testuser
# su - testuser
id
sudo ls /root
exit
# gpasswd -d testuser testadmins
# su - testuser
id
sudo ls /root
exit

2.2.33
# cp /etc/login.defs /etc/login.defs.bak
# cp /etc/default/useradd /etc/default/useradd.bak
# nano /etc/login.defs

2.2.34
# grep 'UID_(MIN|MAX)' /etc/login.defs
# awk -F: '$3 >= 5000 && $3<=6000' /etc/passwd
# nano /etc/login.defs
# grep '^UID_MIN' /etc/login.defs
# useradd configuser1
# id configuser1
# userdel configuser1
# cp /etc/login.defs.bak /etc/login.defs
# grep '^UID_MIN' /etc/login.defs

2.2.35
# awk -F: '$3>=800 && $3<=810' /etc/passwd
# nano /etc/login.defs
# grep -E 'SYS_UID_(MIN|MAX)' /etc/login.defs
# useradd -r sysconfig1
# useradd -r sysconfig2
# useradd -r sysconfig3
# grep sysconfig /etc/passwd
# userdel sysconfig1
# userdel sysconfig2
# userdel sysconfig3
# cp /etc/login.defs.bak /etc/logib.defs

2.2.36
# awk -F: '$3>=7000 && $3<=7100' /etc/group
# nano /etc/login.defs
# grep -E '^GID_(MIN|MAX) /etc/login.defs
# groupadd configgroup1
# getent group configgroup1
# groupdel configgroup1
# cp /etc/login.defs.bak /etc/login.defs

2.2.37
# awk -F: '$3>=850 && $3<=860' /etc/group
# nano /etc/login.defs
# groupadd -r sysgrp1
# groupadd -r sysgrp2
# groupadd -r sysgrp3
# grep sysgrp /etc/group
# groupdel sysgrp1
# groupdel sysgrp2
# groupdel sysgrp3
# cp /etc/login.defs.bak /etc/login.defs

2.2.38
# grep -n CREATE_HOME /etc/login.defs
# nano /etc/login.defs
# useradd homeuser1
# ls -ld /home/homeuser1
# nano /etc/login.defs
# useradd homeuser2
# ls -ld /home/homeuser2
# userdel -r homeuser1
# userdel homeuser2
# cp /etc/login.defs.bak /etc/login.defs

2.2.39
# nano /etc/login.defs
# useradd -r syshome1
# grep '^syshome1:' /etc/passwd
# ls -ld /home/syshome1
# useradd -r -m syshome2
# ls -ld /home/syshome2
# userdel syshome1
# userdel -r syshome2
# cp /etc/login.defs.bak /etc/login.defs

2.2.40
# grep UMASK /etc/login.defs
# grep HOME_MODE /etc/login.defs
# useradd -m modeuser1
# stat -c "%a %n' /home/modeuser1
# nano /etc/login.defs
# useradd -m modeuser2
# stat -c '%a %n' /home/modeuser1 /home/modeuser2
# userdel -r modeuser1
# userdel -r modeuser2
# cp /etc/login.defs.bak /etc/login.defs

2.2.41
# grep USERGROUPS_ENAB /etc/login.defs
# useradd ugtest1
# id ugtest1
# getent group ugtest1
# nano /etc/login.defs
# useradd ugtest2
# id ugtest2
# getent group ugtest2
# userdel ugtest1
# userdel utest2
# cp /etc/login.defs.bak /etc/login.defs

2.2.42
# useradd -D | grep SHELL
# nano /etc/default/useradd
# useradd -D | grep SHELL
# useradd defaultuser1
# grep '^defaultuser1:' /etc/passwd

2.2.43
# useradd -D | grep HOME
# mkdir -p /srv/home
# nano /etc/default/useradd
# useradd -D | grep HOME
# useradd -m defaultuser2
# grep '^defaultuser2:' /etc/passwd
# ls -ld /srv/home/defaultuser2
# userdel defaultuser1
# userdel -r defaultuser2
# cp /etc/default/useradd.bak /etc/default/useradd
# useradd -D
