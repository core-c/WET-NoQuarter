// NoQuarter Shaders

//Console Shader Fix

console-16bit
{
	nopicmip
	{
		map ui/assets/sunset.tga
		rgbGen const ( .35 .35 .35 )
		tcMod scale 1 .5
		tcMod transform 1 0 0 1 0 0.25
		tcMod stretch sin 1.15 0.125 0 0.01
	}
	{
		clampmap ui/assets/background_mask.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
		tcMod stretch sin 1.1 0.05 0 0.01
		tcMod scale 1 .5
		tcMod transform 1 0 0 1 0 0.25
	}
	{
		clampmap textures/skies_sd/colditz_mask.tga
		blendFunc GL_ZERO GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingDiffuse
		tcMod scale 0.85 0.5
		tcMod transform 1 0 0 1 0 0.42
	}
}


