This LUA script is to test the new function "et.G_GetSpawnVar".

The test rule is easy : compare the output of the .lua file when it's executed with
NoQuarter and when whit ETPro. For the best compatibility, the output must to be
the exact same.

To compare the results, run the test with the same map. It will cycle once through
all the entities between entitynum = 64 and entitynum = 1024.

NOTE : don't trust to compare two entities only by entitynum, because it may change
between ETPro and NoQuarter : compare two entities by fields ( for example, field
"model" or field "classname"


DIFFERENCES BETWEEN ETPRO AND NOQUARTER IMPLEMENTATION

1) NoQuarter supports 3 new fields then ETPro : 
		- numPlayers	( int )
		- baseAngle	( vector )
		- baseOrigin	( vector )

2) The field "angle" in ETPro is a NUMERIC field, and in NoQuarter it is a VECTOR

3) If the entity is not in use, the VECTOR fields are returned as STRING type

EXAMPLE

Field		NoQuarter					ETPro

Entity 		219						181
classname 	misc_gamemodel					misc_gamemodel
origin 		x 3208 y 2934 z -326				x 3208 y 2934 z -326
model 		models/mapobjects/pump_sd/pump_animated.md3	models/mapobjects/pump_sd/pump_animated.md3
model2 		nil						nil
spawnflags 	2						2
eflags 		0						0
svflags 	0						0
maxs 		x 0 y 0 z 0					x 0 y 0 z 0
mins 		x 0 y 0 z 0					x 0 y 0 z 0
speed 		0						0
closespeed 	0						0
target 		nil						nil
targetname 	pump1_p2					pump1_p2
message 	nil						nil
popup 		nil						nil
book 		nil						nil
team 		nil						nil
wait 		0						0
random 		0						0
count 		0						0
health 		0						0
light 		nil						nil
dmg 		0						0
angles 		x 0 y 360 z 0					x 0 y 360 z 0
angle 		x 0 y 360 z 0					360
duration 	0						0
rotate 		x 0 y 0 z 0					x 0 y 0 z 0
degrees 	0						0
time 		0						0
skin 		nil						nil
_color 		x 0 y 0 z 0					x 0 y 0 z 0
color 		x 0 y 0 z 0					x 0 y 0 z 0
stylestring 	nil						nil
shader 		nil						nil
key 		0						0
harc 		0						0
varc 		0						0
delay 		0						0
radius 		0						0
missionlevel 	0						0
start_size 	0						0
end_size 	0						0
shard 		0						0
spawnitem 	nil						nil
track 		nil						nil
scriptName 	pump1_p2					pump1_p2
shortname 	nil						nil
constages 	nil						nil
desstages 	nil						nil
partofstage 	0						0
override 	nil						nil
damageparent 	nil						nil
numPlayers 	0						------------
contents 	0						0
clipmask 	0						0
count2 		0						0
baseAngle 	x 0 y 360 z 0					------------
baseOrigin 	x 3208 y 2934 z -326				------------