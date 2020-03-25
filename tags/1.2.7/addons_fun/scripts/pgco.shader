// ***** Stock Models START *****
// ***** Goldbox START *****
models/mapobjects/goldbox_sd/pgcogoldbox
{
	cull disable
	surfaceparm nomarks
	surfaceparm metalsteps
	{
		map models/mapobjects/goldbox_sd/goldbox.tga
		rgbGen lightingDiffuse
	}
	{
		map textures/effects/objectivegreen.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}

models/multiplayer/gold/pgcogold
{
	cull disable
	surfaceparm nomarks
	surfaceparm metalsteps
//	{
//		map models/multiplayer/gold/gold.tga
//		rgbGen lightingDiffuse
//	}
	{
		map models/multiplayer/gold/gold.tga
		rgbGen lightingdiffuse
	}
	{
		map textures/effects/envmap_gold.tga
		blendFunc GL_ONE GL_ONE
		tcMod scale 2.25 1.25
		tcGen environment
		rgbGen lightingdiffuse
	}
	{
		map textures/effects/objectivegreen.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}
// ***** Goldbox END *****

// ***** Portable Radar START *****
models/mapobjects/portable_radar_sd/pgcoportable_radar_sd
{
	cull disable
	surfaceparm nomarks
	surfaceparm metalsteps
	{
	         map textures/effects/envmap_slate.tga
	         rgbGen lightingdiffuse
	         tcGen environment
	}
	{
	         map models/mapobjects/portable_radar_sd/portable_radar_sd.tga
	         blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
	         rgbGen lightingdiffuse
	}
	{
		map textures/effects/objectivegreen.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcmod rotate 30
		tcmod scroll 1 .1
	}
}
// ***** Portable Radar END *****

// ***** Secret Docs START *****
// Box06 + Box07 + Box08
models/multiplayer/secretdocs/pgcopaperstack2
{
	cull disable
	surfaceparm nomarks
	{
		map models/multiplayer/secretdocs/paperstack2.tga
		rgbGen lightingDiffuse
	}
	{
		map textures/effects/objectivegreen.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}

// Box05
models/multiplayer/secretdocs/pgcowood_m12
{
	cull disable
	surfaceparm nomarks
	{
		map textures/wood/wood_m12.tga
		rgbGen lightingDiffuse
	}
	{
		map textures/effects/objectivegreen.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}

// Box04
models/multiplayer/secretdocs/pgcoclipboard
{
	cull disable
	surfaceparm nomarks
	{
		map models/multiplayer/secretdocs/clipboard.tga
		rgbGen lightingDiffuse
	}
	{
		map textures/effects/objectivegreen.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}

// Box11 + Box12
models/multiplayer/secretdocs/pgcopaperstack
{
	cull disable
	surfaceparm nomarks
	{
		map models/multiplayer/secretdocs/paperstack.tga
		rgbGen lightingDiffuse
	}
	{
		map textures/effects/objectivegreen.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}
// ***** Secret Docs END *****
// ***** Stock Models END *****

// ***** Custom Models START *****
// ***** Custom Models END *****