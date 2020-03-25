This LUA script is to test the new function "et.G_SetSpawnVar".

The test just search for th Axis command post entity in Oasis map, and replace the
popup text a player see when he is near it. After the string is replaced,
we check again if the value changed with et.G_GetSpawnVar.


DIFFERENCES BETWEEN ETPRO AND NOQUARTER IMPLEMENTATION

1) If the value is a string, ETPro allow it to be a longer string then the standard one,
and NoQuarter can handle at least strings with the same size of the original, or shorter.
This is pretty bad, let's remember to check this in future.