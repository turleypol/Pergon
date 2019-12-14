**********************************************************
***
***   Fishing OSI style plus+ Version 1.0.1
***
**********************************************************
***   Last Updated: 04/21/00
***             By: Cassandra White
**********************************************************

Note: I am not the origanal scripter of this package by anymeans. I have just accepted the responsability for 
putting the script back into working order, and made myself available for any questions or updates/suggestions
that may occur. - Cassandra White -

How to install this fishing pkg:

1) Unzip all of the files into pkg\std\fishing

2) Copy fish.cfg to the regions folder

3) Manualy add the resource entry to the POL\regions\resource.cfg.

4) You should recompile fishing.src, sosbottle.src, and sosmessage.src just to verify that you are not 
having problems. If you are having any problems, you can reach me at casswhite@home.com

Note: This version of 'Fishing101' has been tested with Distro 88s and 88c, to have no problems after 30 days.
      The code is still a real mess, however this is a working copy. Next release will use more efficient code.

**********************************************************
***
***   Notes from 'Tom':
***   Thanks to DreamWeaver and Louds for all of the work that they did on the fishing scripts that I 
***   used in my version.  You guys really helped me to learn a lot about escript with your work.
***
***********************************************************

Have fun!

Tomas (tbaffy@voyager.net)

// ***	I must warn you that I have altered the base code significantly.  There
// ***	is actually little resemblance between my script and what I started with.
// ***
// ***  I fixed the bug that causes water tiles to be misidentified so that
// ***	the "That's not water!" message was being sent even when a valid water
// ***	tile was selected as the target	
// ***
// ***	The script currently allows only tiles that are complete flat water
// ***	squares to be targeted for fishing (no rocks, no waterfalls, no partial
// ***	water squares can be targeted).  I did this on purpose to allow for the 
// ***	splash animation to be use effectively later.
// ***
// ***	I removed all references to the woodcutting script that was obviously the
// ***	basis of this script.  All references now are in context with fishing only.
// ***
// ***	I added a new constant FISHING_RANGE to allow for the targetable range of
// ***	the fishing skill to be changed easily.
// ***
// ***	I added several constants to define the skill level required to catch
// ***  different types of objects.  The script currently allows a fisherman to
// ***	catch:  big fish, footwear, little fish, treasure maps, and SOS bottles.
// ***	The skill level constants also impact the percentage chance of getting
// ***	each type of object when a fishing attempt is made.
// ***	
// ***	I added the splash sound effect while fishing
// ***
// ***	I changed the script to execute the casting animation only once
// ***
// ***	I added the splash animation to the target square
// ***
// ***	I made a fishing attempt have a consistent 5 second elapsed time
// ***
// ***	I altered the messages that are issued to reflect what type of object
// ***	was caught in the fishing attempt.
// ***
// ***	I changed the error message when a player targets an invalid square
// ***	during a fishing attempt.  It used to say that the square was not water,
// ***	but this doesn't work well because I have made some water squares invalid
// ***	targets for fishing (rocks, waterfalls, partial water squares) since
// ***	they will not look right with the splash animation when it is implemented.
// ***	The new error message just says that "You cannot fish there!" instead.
// ***
// ***	I separated the line of sight check from the target range check so that
// ***	a different error message could be issued.  All three target vailidity
// ***	checks were incorporated into one function to check if the target square
// ***	is valid.
// ***
// ***	Skill gain is now related to the difficulty of harvesting the resource
// ***
// ***	The GAIN_MULTIPLIER constant was created to control skill advancement.
// ***	It is multiplied by the harvest difficulty to calculate skill gain.
// ***
// ***	The fish resource has been added and a fish.cfg file created for the 
// ***	regions folder.  This is managed just like all other regional resources.
// ***	The fishing skill was altered to use the standard resource harvesting 
// ***	system.
// ***
// ***  I used the scripts that Louds published as a basis to implement SOS bottles
// ***  much of what Louds did I kept intact, but I made some alterations to the 
// ***  process.  I do not use a tile to mark an SOS site.  I check that the
// ***	location that is targeted for fishing, is within the fishing range of an
// ***	SOS message in your pack instead.
// ***
// ***	I added the ability to get miscellaneous treasures while fishing up an 
// ***	SOS message.  The routine currently supports fishing up seashells,
// ***	paintings, tapestries, rolls of metal wire, and pillows.
// ***
// ***	I moved the constants to fishing.inc to keep them consistent across modules
// ***
// ***	I fixed the splash animation by using the splash code from Yuri's version of 
// ***	the fishing script.  Thanks Yuri!
// ***
// ***	Fixed problem with landtile based targets are not yielding fish.  
// ***
// ***	Added in more creature types to spawn when critical failure occurs.  The
// ***  routine will now spawn sea serpents, water elementals, alligators, and
// ***	walrus upon critical failure.
// ***
// ***	Changed small fish into magical fish.  There are 8 types of magical fish
// ***	they cast bless, agility, cunning, strength, cure, heal, invisibility,
// ***	and night sight.  The effectiveness of these magic effects can be adjusted
// ***	through cprops in the itemdesc.cfg in the std/pkg/fishing folder.
// ***