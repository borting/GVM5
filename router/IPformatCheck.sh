#! /bin/sh
# ref:
# 	http://www.linuxquestions.org/questions/linux-software-2/
#		shell-script-ip-address-format-check-219187/

regex="\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\b"

CHECK=`echo $1 | egrep $regex`
if [[ "$?" -eq 0 ]]
then
	echo "Correct IP address"
else
	echo "Incorrect IP address, please try again: "
fi

