#!%PYTHON_BANGPATH%

# assign_sched_code.py
#
# RivWebPyApi test script for Rivendell
#
#  Test the AssignSchedCode Web API call
#
#   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License version 2 as
#   published by the Free Software Foundation.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public
#   License along with this program; if not, write to the Free Software
#   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#

import getpass
import rivwebpyapi
import sys
def eprint(*args,**kwargs):
    print(*args,file=sys.stderr,**kwargs)

url='';
username=''
password=''
cart_number=0
sched_code=''

#
# Get login parameters
#
usage='assign_sched_code --url=<rd-url> --username=<rd-username> --cart-number=<num> --sched-code=<code> [--password=<passwd>]'
for arg in sys.argv:
    f0=arg.split('=')
    if(len(f0)==2):
        if(f0[0]=='--url'):
            url=f0[1]
        if(f0[0]=='--username'):
            username=f0[1]
        if(f0[0]=='--password'):
            password=f0[1]
        if(f0[0]=='--cart-number'):
            cart_number=int(f0[1])
        if(f0[0]=='--sched-code'):
            sched_code=f0[1]

if(not password):
    password=getpass.getpass()
if((not url)or(not username)):
    eprint(usage)
    sys.exit(1)
if(cart_number==0):
    eprint('you must supply "--cart-number"')
    sys.exit(1)
if(not sched_code):
    eprint('you must supply "--sched-code"')
    sys.exit(1)

#
# Execute
#
webapi=rivwebpyapi.rivwebpyapi(url=url,username=username,password=password)
try:
    webapi.AssignSchedCode(cart_number=cart_number,sched_code=sched_code)
except rivwebpyapi.RivWebPyError as err:
    eprint('*** ERROR ***')
    eprint('Response Code: '+str(err.responseCode))
    eprint('ErrorString: '+str(err.errorString))
    eprint('*************')
    eprint('')
    sys.exit(1)
