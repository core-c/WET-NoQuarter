textures/xposed/terrain_base
{
	nopicmips
	q3map_lightmapaxis z
	q3map_lightmapmergable
	q3map_tcGen ivector ( 512 0 0 ) ( 0 512 0 )
}

textures/xposed/terrain_0
{
	nopicmip
	q3map_baseshader textures/xposed/terrain_base
	qer_editorimage textures/desert_sd/road_dirty_gravel.tga
	surfaceparm gravelsteps
	surfaceparm landmine
	{
		map textures/desert_sd/road_dirty_gravel.tga
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/xposed/terrain_1
{
	nopicmip
	q3map_baseshader textures/xposed/terrain_base
	qer_editorimage textures/temperate_sd/dirt_m03icmp_brown.jpg
	{
		map textures/temperate_sd/dirt_m03icmp_brown.jpg
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/xposed/terrain_2
{
	nopicmip
	q3map_baseshader textures/xposed/terrain_base
	qer_editorimage textures/rubble/debri_m01.jpg
	surfaceparm landmine
	surfaceparm gravelsteps		
	{
		map textures/rubble/debri_m01.jpg
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/xposed/terrain_0to1
{
	nopicmip
	q3map_baseshader textures/xposed/terrain_base
	qer_editorimage textures/desert_sd/road_dirty_gravel.tga
	surfaceparm gravelsteps
	surfaceparm landmine
	{
		map textures/desert_sd/road_dirty_gravel.tga
	}
	{
		map textures/temperate_sd/dirt_m03icmp_brown.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/xposed/terrain_0to2
{
	nopicmip
	q3map_baseshader textures/xposed/terrain_base
	qer_editorimage textures/desert_sd/road_dirty_gravel.tga
	surfaceparm gravelsteps
	surfaceparm landmine
	{
		map textures/desert_sd/road_dirty_gravel.tga
	}
	{
		map textures/rubble/debri_m01.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}

textures/xposed/terrain_1to2
{
	nopicmip
	q3map_baseshader textures/xposed/terrain_base
	qer_editorimage textures/temperate_sd/dirt_m03icmp_brown.jpg
	surfaceparm gravelsteps
	surfaceparm landmine
	{
		map textures/temperate_sd/dirt_m03icmp_brown.jpg
	}
	{
		map textures/rubble/debri_m01.jpg
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		alphaGen vertex
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
}
textures/xposed/xposed_sky
{
	nocompress
	qer_editorimage textures/skies/topclouds.tga
	q3map_lightrgb 0.8 0.9 1.0
	q3map_sun 1 .96 .87 140 140 28
	q3map_skylight 60 3
	q3map_nofog
	
	surfaceparm noimpact
	surfaceparm nolightmap
	surfaceparm nodlight
	surfaceparm sky
	
	skyparms textures/skies_sd/wurzburg_env/sky 512 -
 
	sunshader textures/skies_sd/batterysunfog
 
	
	{	fog off
		map textures/skies_sd/battery_clouds.tga
		tcMod scale 2.5 2.5
		tcMod scroll 0.0015 -0.003
		rgbGen identityLighting
	}

	{
		fog off
		clampmap textures/skies_sd/battery_mask_ydnar.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		tcMod scale 0.956 0.956
		tcMod transform 1 0 0 1 -1 -1
		// rgbGen identityLighting
		//rgbGen const ( 0.6 0.6 0.6 ) 
		rgbGen const ( 0.4 0.4 0.4 ) 
	}
}

textures/xposed/brick_wall_04
{
	implicitMap -
}


textures/xposed/brick_wall_08
{
	implicitMap -
}

textures/xposed/brick_wall_09
{
	implicitMap -
}

textures/xposed/flat_rocks
{
	implicitMap -
}

textures/xposed/brick_wall_01
{
	implicitMap -
}

textures/xposed/brick_wall_04
{
	implicitMap -
}
textures/xposed/big_blocks
{
	implicitMap -
}

textures/xposed/brick_wall_05
{
	implicitMap -
}

textures/xposed/oily_old_crate_1
{
	implicitMap -
	nopicmip
}

textures/xposed/oily_old_crate_2
{
	implicitMap -
	nopicmip
}

textures/xposed/oily_old_crate_3
{
	implicitMap -
	nopicmip
}

textures/xposed/oily_old_crate_4
{
	implicitMap -
	nopicmip
}

textures/xposed/oily_old_crate_5
{
	implicitMap -
	nopicmip
}

textures/xposed/oily_old_crate_6
{
	implicitMap -
	nopicmip
}

textures/xposed/oily_old_crate_7
{
	implicitMap -
	nopicmip
}

textures/xposed/oily_old_crate_8
{
	implicitMap -
	nopicmip
}


textures/xposed/grass1
{
	implicitMap -
	surfaceparm grasssteps
	surfaceparm landmine
}

textures/xposed/window1
{
	implicitMap -
}

textures/xposed/window1_broken
{
	implicitMap -
}

textures/xposed/window29
{
	implicitMap -
}

textures/xposed/window30
{
	implicitMap -
}

textures/xposed/roughbrickx
{
	implicitMap -
}

textures/xposed/food_16
{
	nopicmip
	implicitMap -
}

textures/xposed/food_28
{
	nopicmip
	implicitMap -
}


textures/xposed/roof_broken
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}

textures/xposed/roof_broken2
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}


textures/xposed/roof_broken3
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}


textures/xposed/brick_wall_10
{
	implicitMap -
}


textures/xposed/xfloor
{
	implicitMap -
}

textures/xposed/church1
{
	implicitMap -
}

textures/xposed/church1d
{
	implicitMap -
}



textures/xposed/water01
{
	nocompress
	qer_editorimage textures/xposed/water01.tga
	qer_trans .5
	q3map_globaltexture
	surfaceparm trans
	surfaceparm nonsolid
	surfaceparm water
	surfaceparm nomarks
	cull disable
	nopicmip
	nofog

	waterfogvars ( 0.51 0.53 0.55 ) 0.2
		{ 
		fog on
		map textures/xposed/water01.tga
		blendFunc blend
		alphaFunc GE128
		depthWrite
		rgbgen identity
		tcmod scale 0.5 0.5
		tcmod scroll -.02 .001
	}
	
	{ 
		fog on
		map textures/liquids_sd/seawall_ripple1.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.3 0.02 0 0.25 
		tcmod scale 0.01 0.01
		tcmod scroll -.001 -.0002
	}
	
	{ 
		fog on
		map textures/liquids_sd/seawall_ripple1.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.1 0.03 0 0.4
		tcmod scale 1 1
		tcmod scroll -.005 -.001
	}
	{ 
		fog on
		map textures/xposed/xposed_shimshim.tga
		blendFunc GL_ONE GL_ONE
		rgbGen wave sin 0.4 0.02 0 0.3
		tcmod transform 0 1.5 1 1.5 2 1
		tcmod scroll .005 -.001
	}
}


textures/xposed/wall_alpha
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}


textures/xposed/poster1
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}

textures/xposed/poster2
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}

textures/xposed/poster3
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}

textures/xposed/poster4
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}

textures/xposed/muv158
{
	implicitMap -
}

textures/xposed/sup029
{
	implicitMap -
}

textures/xposed/sup022
{
	implicitMap -
}

textures/xposed/sup027
{
	implicitMap -
}

textures/xposed/sup050
{
	implicitMap -
}

textures/xposed/muv161
{
	implicitMap -
}

textures/xposed/burn@ringa
{
	qer_editorimage textures/xposed/burn-1@ringa.tga
	nopicmip
	{
		animmap 3 textures/xposed/burn-1@ringa.tga textures/xposed/burn-1@ringa.tga textures/xposed/burn-2@ringa.tga 
	}
	{
		animmap 1 textures/xposed/burn-1@ringa.tga textures/xposed/burn-1@ringa.tga textures/xposed/burn-2@ringa.tga 
		rgbGen const ( 0 0 0 )
	}
	{
		animmap 5 textures/xposed/burn-1@ringa.tga textures/xposed/burn-2@ringa.tga textures/xposed/burn-1@ringa.tga 
		//blendfunc gl_dst_color gl_src_color
		alphaGen wave sin 1 1 1 1 
	}
	{
		animmap 3 textures/xposed/burn-2@ringa.tga textures/xposed/burn-1@ringa.tga textures/xposed/burn-1@ringa.tga 
		//blendfunc gl_dst_alpha gl_dst_color
	}
	{
		animmap 5 textures/xposed/burn-2@ringa.tga textures/xposed/burn-1@ringa.tga textures/xposed/burn-1@ringa.tga 
		//blendfunc gl_dst_alpha gl_dst_color
	}
}

textures/xposed/burntwood
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}


textures/xposed/burn_floor
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}

textures/xposed/wine
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}

textures/xposed/bux
{
	cull disable
	nomipmaps
	nopicmip
	surfaceparm nomarks
	surfaceparm alphashadow
	surfaceparm metalsteps
	surfaceparm pointlight
	surfaceparm trans
	implicitMask -
}