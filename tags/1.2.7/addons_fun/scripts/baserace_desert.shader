textures/desert_sd/sand_wave_desert
{
    q3map_nonplanar
    qer_editorimage textures/desert_sd/sand_wave_desert.tga
    surfaceparm landmine
    surfaceparm gravelsteps
    
    {
        map $lightmap
        rgbGen identity
    }
    {
		map textures/desert_sd/sand_wave_desert.tga
        blendFunc filter
    }

}

textures/radar/desert_fog
{
	qer_editorimage textures/sfx/fog_grey1.tga
	
	surfaceparm nodraw
	surfaceparm nonsolid
	surfaceparm trans
	surfaceparm fog
	
	fogparms ( 0.75 0.70 0.6 ) 3192
}

textures/baserace_desert/smily
{
polygonOffset
surfaceparm trans
//	implicitBlend textures/baserace_desert/smily.tga
      {
		map textures/baserace_desert/smily.tga
		blendfunc blend
	}
}


textures/lights/light_m30
{
	qer_editorimage textures/lights/light_m30.tga
	q3map_lightimage textures/lights/light_m30.tga
	q3map_surfacelight 5000
	surfaceparm nomarks
	{
		map $lightmap
		rgbGen identity
	}
	{
		map textures/lights/light_m30.tga
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map textures/lights/light_m30.tga
		blendFunc GL_ONE GL_ONE
	}
}

textures\baserace\matbox_blue
{
{
		map textures\baserace\matbox_blue.tga
		rgbGen lightingDiffuse
	} 
	{
		map textures/effects/objectiveblue.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}
textures\baserace\matbox_red
{
{
		map textures\baserace\matbox_red.tga
		rgbGen lightingDiffuse
	} 
	{
		map textures/effects/objectivered.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
                tcmod rotate 30
                tcmod scroll 1 .1
	}
}