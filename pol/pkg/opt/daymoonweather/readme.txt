I've always hankered for a more realistic night & day system. This system takes into account
various factors while doing the normal night & day cycle. At night, the lightlevel is determined
by the 2 moons Felucca & Trammel, effectively giving the world moonlight. There are also different
weather effects which govern the daylight. A sunny day is obviously brighter than an overcast or
thundery day. Unfortunately i had to remove the actual rain & snow effects as i just couldn't get
them to co-operate :(, but i left the lightning and thunder in for your amusement :).
Transition times and levels adjust automatically to the moons & weather so you won't get a
transition to daylight and then suddenly darken to account for the weather, fluid, nice.
Also added seasons to affect weather, more likely for bad weather in winter and good weather in
summer. Spyglasses can be used to determine what kind of light level to expect for that evening
(for budding astronomers, hehe).

This isn't based on any given ultima world. Basically when Trammel phases 8 times (full cycle),
Felucca phases once. When Felucca phases 8 times (full cycle) the season changes. Making the UO
year 256 days long. Also, transition time in the days.cfg is no longer used, it's now 1/4 of the
total day length.

Changes to be made:
You will probably have to either edit or delete the present item.cfg entries for a telescope to
use the spyglass script. The rest sit in a pkg.

Big thanks to Rac and the rest of the scriptforum for filling the gaps, MatW whoes spyglass
script is still in one piece and needed and whomever wrote the original scripts that i have
added to, crucified and generally mullered.

version 1.01
fixed slight light error when weather changes during transition
fixed kludge with lightning, now runs as seperate script and prevented from calling multiple times

version 1.02
finally fixed rain & snow! wohoo! make sure your "Background" setting is in the
 \regions\weather.cfg as it uses the "Background" for all weather effects. (AssignRect does not
work, sorry).

version 1.03
fixed global setup for lunar cycle, oops, my bad