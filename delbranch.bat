set git="C:\Program Files\Git\bin\git.exe" 
set branch=devmerge3


rem  delete branch locally
%git% branch -d %branch%

pause

rem // delete branch remotely
%git% push origin --delete %branch%

pause
