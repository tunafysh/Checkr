import os, platform, sys, pexpect

osname = platform.os.name

if osname == "nt":
    print("put windows build code here")
elif osname == "posix":
    print("put linux build code here")
else:
    print("Unsupported system.")
 
