Static Housing
by: Bishop Ebonhand
Version: 1.0

this package contains everything you need to turn static homes into player owned housing.. 
the included scripts allow you to create an ownership deed that can be given to players or
sold on a vendor. when a player double clicks on the deed, it transfers the ownership info 
from the deed along with the characters serial number to the house sign.. this is an 
automatic process and the player doesnt have to be near the sign for this to work as the 
script seeks out the sign by serial number and applies the properties to it directly..
when a static house is created, the coordinates chosen by the creating staff member are used
to determine the lockdown and secure container counts.. This is very important to remember, as
the script assumes all houses to be square. so depending on the shape of the house, it is 
possible that players can lockdown items and create secure containers outside their house in 
theese areas.. Also keep in mind that if a house is a multi-level house, when targeting the
northwest corner, you must target it at a Z axis that is equal to the roof of the highest 
level of a house.. If a house is not designed in such a way that this is possible, you must
create an item and elevate it to the proper Z level (height is 20 per level of the house)...
then target the item as the northwest corner placement. likewise, the southeast corner must 
also be targeted to include the entire house in the "square" and must also be targeted at the
lowest Z axis of the house or the lockdown and secure containers will calculate or place 
correctly.. After initially taking ownership of a house, a player must use the house sign once
before its listener script activates. the listener script will intercept the following commands:

"i wish to lock this down"
"i wish to release this"
"create secure container east"
"create secure container south"

adding friends to the house is done through the house sign, as well as removing friends.. 
the sign interface also allows players to change the front door locks (house sign must be placed 
within 2 squares of the front doors for its "change house locks" function to operate properly) or
to individually key each interior door of the house.. when the player double clicks on the 
ownership deed the deed automatically keys all house doors to the same key.. upon transfering 
ownership, the house deed moves to the players bank so that it may be later sold or traded to
other players.. it is important that players keep this deed safe as anyone that uses the deed
will gain ownership of the house.. 

secure containers may only be accessed by any players on the house owners account, or any friends
of the house.. the friending function is pretty careful about not friending NPCs or ITEMS.. 
transfering a house to someone else will not reset its lockdowns or secure containers (this is done 
to prevent an exploit to get extra secure containers and lockdowns by passing a deed back and forth)
the new_items.cfg file contains the 2 items you will need to add to the pkg\std\housing\itemdesc.cfg 
file.. (this will require a server restart in order to take effect)
to install this package, place the staticdeed.src/ecl into the scripts\textcmd\(GM or ADMIN) folder
and transferdeed.src/ecl, staticsign.src/ecl, and staticsigncontrol.src/ecl scripts into pkg\std\housing..

move recall.src and gate.src into pkg\std\spells and compile (if you are using modified recall scripts, 
you will have to just take the static checking from theese scripts and move them into your own scripts..
the recall script is much different than the default script in operation, so please test it and see if you 
like its new features.. if not, just take the static house checks and migrate them into your recall script..
if you decide to go with my updated recall script, you will have to also use part of the enclosed logon.src
to prevent problems with untimely logouts.. if not, then you can discard the file completely.. 
move logon.src/ecl into scripts\misc

As always any questions/comments/concerns can be sent to: bish0p@cfl.rr.com
I hope you all enjoy this package and all the benefits it offers.. 