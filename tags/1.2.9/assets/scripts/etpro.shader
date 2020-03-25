// All credits go to ETPro stuff, thanks Reyalp for providing us with these shaders!

// sharp crosshairs
// TODO we could save a lot of shaders if we were smart about this
gfx/2d/crosshairk
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairk.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// nothing
gfx/2d/crosshairk_alt
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshaira_alt.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// slightly modified version of crosshair b so it centers
gfx/2d/crosshairl
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairl.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// sharp center version of b_alt
gfx/2d/crosshairl_alt
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairl_alt.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// copy of c
gfx/2d/crosshairm
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairc.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// sharp version of b_alt
gfx/2d/crosshairm_alt
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairp.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// copy of d
gfx/2d/crosshairn
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshaird.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// sharp version of d_alt
gfx/2d/crosshairn_alt
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairp.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// copy of e
gfx/2d/crosshairo
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshaire.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// sharp version of e_alt
gfx/2d/crosshairo_alt
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairp.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

//sharp dot
gfx/2d/crosshairp
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairp.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
// blurs even less, but artifacts at many sizes
//		alphaFunc GT0
		rgbGen vertex
	}
}

// nothing
gfx/2d/crosshairp_alt
{
	nocompress
	nopicmip
	{
		map gfx/2d/crosshairf_alt.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}
