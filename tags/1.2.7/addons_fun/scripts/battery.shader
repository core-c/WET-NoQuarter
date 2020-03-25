// ocean fog water
textures/battery/fog_water
{
	qer_editorimage textures/common/fog_water.tga
	surfaceparm nodraw
  	surfaceparm nonsolid
  	surfaceparm trans
  	surfaceparm water
}

// abstract shader for subclassed shaders
textures/battery/ocean_base
{
	qer_editorimage textures/liquids_sd/seawall_ocean.tga
	qer_trans 0.75
	q3map_tcGen ivector ( 512 0 0 ) ( 0 512 0 )
	q3map_globalTexture
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm nomarks
	surfaceparm lightfilter
	surfaceparm pointlight
	nopicmip
}

// nonsolid terrain shader
textures/battery/ocean_terrain
{
	qer_editorimage textures/common/terrain_nonsolid.tga
	q3map_terrain
	qer_trans .5
	surfaceparm nodraw
  	surfaceparm nonsolid
  	surfaceparm trans
  	nopicmip
  	
}

// subclassed ocean shaders
textures/battery/ocean_0
{
	q3map_baseshader textures/battery/ocean_base
	cull none
	deformVertexes wave 1317 sin 0 2.5 0 0.15
 	deformVertexes wave 317 sin 0 1.5 0 0.30
	
	{
		map textures/liquids_sd/seawall_specular.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
		tcGen environment
		depthWrite
	}
		
	{ 
		map textures/liquids_sd/sea_bright_na.tga
		blendFunc blend
		rgbGen identity
		alphaGen const 0.8
		tcmod scroll 0.005 0.03
	}
	
}

textures/battery/ocean_1
{
	q3map_baseshader textures/battery/ocean_base
	cull none
	deformVertexes wave 1317 sin 0 2.5 0 0.15
 	deformVertexes wave 317 sin 0 1.5 0 0.30
	
	{
		map textures/liquids_sd/seawall_specular.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
		tcGen environment
		depthWrite
	}
	
	{ 
		map textures/liquids_sd/sea_bright_na.tga
		blendFunc blend
		rgbGen identity
		alphaGen const .8
		tcmod scroll 0.005 0.03

	}
	{
		map textures/liquids_sd/seawall_foam.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen wave sin 0.2 0.1 0 0.2
		alphaGen vertex
		tcMod turb 0 0.05 0 0.15
		tcmod scroll -0.01 0.08
	}
	{ 
		map textures/liquids_sd/seawall_foam.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen wave sin 0.15 0.1 0.1 0.15
		alphaGen vertex
		tcMod turb 0 0.05 0.5 0.15
		tcmod scroll 0.01 0.09
	}
	
}

textures/battery/ocean_0to1
{
	q3map_baseshader textures/battery/ocean_base
	cull none
	deformVertexes wave 1317 sin 0 2.5 0 0.15
 	deformVertexes wave 317 sin 0 1.5 0 0.30 	
	
	{
		map textures/liquids_sd/seawall_specular.tga
		blendFunc GL_ONE GL_ONE
		rgbGen vertex
		tcGen environment
		depthWrite
	}
	
	{ 
		map textures/liquids_sd/sea_bright_na.tga
		blendFunc blend
		rgbGen identity
		alphaGen const .8
		tcmod scroll 0.005 0.03
	}
	{
		map textures/liquids_sd/seawall_foam.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen wave sin 0.2 0.1 0 0.2
		alphaGen vertex
		tcMod turb 0 0.05 0 0.15
		tcmod scroll -0.01 0.08
	}
	{ 
		map textures/liquids_sd/seawall_foam.tga
		blendFunc GL_SRC_ALPHA GL_ONE
		rgbGen wave sin 0.15 0.1 0.1 0.15
		alphaGen vertex
		tcMod turb 0 0.05 0.5 0.15
		tcmod scroll 0.01 0.09
	}
	
}

textures/battery/rock_graynoise
{
    q3map_nonplanar
    q3map_shadeangle 180
    //q3map_tcmodscale 1.5 1.5
    qer_editorimage models/mapobjects/rocks_sd/rock_temperate2_big.tga
    {
        map $lightmap
        rgbGen identity
    }
    {
        map models/mapobjects/rocks_sd/rock_temperate2_big.tga
        blendFunc filter
    }
    {
    	map textures/detail_sd/sanddetail.tga
    	blendFunc GL_DST_COLOR GL_SRC_COLOR
    	detail
    	tcMod scale 6 6
    }
}

textures/battery/sand_disturb
{
    q3map_nonplanar
    q3map_shadeangle 180
    //q3map_tcmodscale 1.5 1.5
    qer_editorimage textures/temperate_sd/sand_patchnoise.tga
    surfaceparm landmine
    surfaceparm gravelsteps
    
    {
        map $lightmap
        rgbGen identity
    }
    {
	map textures/temperate_sd/sand_patchnoise.tga
        blendFunc filter
    }
    {
    	map textures/detail_sd/sanddetail.tga
    	blendFunc GL_DST_COLOR GL_SRC_COLOR
    	detail
    	tcMod scale 6 6
    }
}

textures/battery/terrain_base
{
	q3map_tcGen ivector ( 256 0 0 ) ( 0 256 0 )
	q3map_lightmapsize 512 512
	q3map_lightmapMergable
	q3map_lightmapsamplesize 16
	q3map_lightmapaxis z
	q3map_shadeangle 179
}

textures/battery/terrain_0
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	
	{

		map textures/temperate_sd/sand_patchnoise.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}

textures/battery/terrain_1
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/grass_dense1.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}

textures/battery/terrain_2
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/dirt3.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}

textures/battery/terrain_3
{
	q3map_baseshader textures/battery/terrain_base
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}

textures/battery/terrain_4
{
	q3map_baseshader textures/battery/terrain_base
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}

textures/battery/terrain_5
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/grass_path1.tga
		rgbgen identity
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}


textures/battery/terrain_0to1
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/sand_patchnoise.tga
		rgbgen identity
		alphaGen vertex
		
	}
	{
		map textures/temperate_sd/grass_dense1.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}

textures/battery/terrain_0to2
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/sand_patchnoise.tga
		rgbgen identity
		alphaGen vertex
		
	}
	{
		map textures/temperate_sd/dirt3.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}
}

textures/battery/terrain_0to3
{
	q3map_baseshader textures/battery/terrain_base
	{
		map textures/temperate_sd/sand_patchnoise.tga
		rgbgen identity
		alphaGen vertex
		
	}
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}

	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_0to4
{
	q3map_baseshader textures/battery/terrain_base
	{
		map textures/temperate_sd/sand_patchnoise.tga
		rgbgen identity
		alphaGen vertex
		
	}
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}

	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_0to5
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/sand_patchnoise.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map textures/temperate_sd/grass_path1.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}


textures/battery/terrain_1to2
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/grass_dense1.tga
		rgbgen identity
		alphaGen vertex

	}
	{
		map textures/temperate_sd/dirt3.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_1to3
{
	q3map_baseshader textures/battery/terrain_base
	{
		map textures/temperate_sd/grass_dense1.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_1to4
{
	q3map_baseshader textures/battery/terrain_base
	{
		map textures/temperate_sd/grass_dense1.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_1to5
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/grass_dense1.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map textures/temperate_sd/grass_path1.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}



textures/battery/terrain_2to3
{
	q3map_baseshader textures/battery/terrain_base
	{
		map textures/temperate_sd/dirt3.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_2to4
{
	q3map_baseshader textures/battery/terrain_base
	{
		map textures/temperate_sd/dirt3.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_2to5
{
	q3map_baseshader textures/battery/terrain_base
	surfaceparm landmine
	surfaceparm gravelsteps
	{
		map textures/temperate_sd/dirt3.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map textures/temperate_sd/grass_path1.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_3to4
{
	q3map_baseshader textures/battery/terrain_base
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_3to5
{
	q3map_baseshader textures/battery/terrain_base
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map textures/temperate_sd/grass_path1.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/terrain_4to5
{
	q3map_baseshader textures/battery/terrain_base
	{
		map models/mapobjects/rocks_sd/rock_temperate2_big.tga
		rgbgen identity
		alphaGen vertex
	}
	{
		map textures/temperate_sd/grass_path1.tga
		rgbgen identity
		alphaGen vertex
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
	}
	{
		map $lightmap
		blendFunc GL_DST_COLOR GL_ZERO
	}
	{
		map textures/detail_sd/sanddetail.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		detail
		tcMod scale 4 4
	}

}

textures/battery/water_nodraw
{
	qer_editorimage textures/common/nodraw.tga
	surfaceparm nodraw
  	surfaceparm nonsolid
  	surfaceparm trans
  	surfaceparm water
}

textures/battery/water_fog
{
	qer_editorimage textures/common/fog_water.tga
	surfaceparm nodraw
  	surfaceparm nonsolid
  	surfaceparm trans
  	surfaceparm fog
  	
  	fogparms ( 0.3137 0.36 0.4039 ) 256
}