models/players/common/specGlow
{
	deformVertexes wave 1000 sin 1.75 1 0 .5
	cull none
	{
		map $whiteimage
		blendFunc gl_one gl_one
		rgbGen entity
	}
}

models/players/common/specGlowWeapon
{
	deformVertexes wave 1000 sin 1.75 1 0 .5
	cull none
	{
		map $whiteimage
		blendFunc gl_one gl_one
		rgbGen entity
	}
}

onScreenColor
{
	{
		map $whiteimage
		blendFunc gl_one gl_src_color
		rgbGen vertex
	}
}

models/players/hud/adrenFace
{
	{
		map models/players/hud/adren_faceglow.jpg
		blendFunc gl_one gl_one
	}
}
