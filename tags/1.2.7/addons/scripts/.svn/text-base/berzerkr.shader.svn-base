// NO QUARTER ADDITIONS
// --------------------
//
// Content:
// ~~~~~~~~
// + Helmet Additions
// + Playerskins
// + Reflective Effect for Teamspecific Stuff
// + Waving Grass (by RayBan)
//



// Helmet Additions

models/players/temperate/axis/soldier/gasmask
{
	cull none
	implicitBlend -
	surfaceparm trans
	surfaceparm pointlight
	surfaceparm alphashadow
	{
		map models/players/temperate/axis/soldier/gasmask.tga
		rgbGen lightingDiffuse
	}
}

models/players/temperate/axis/cvops/camouflage
{
	cull twosided
	implicitBlend -
	surfaceparm trans
	surfaceparm pointlight
	surfaceparm alphashadow
	{
		map models/players/temperate/axis/cvops/camouflage.tga
		rgbGen lightingDiffuse
		depthWrite
	}
}



// Playerskins

models/players/temperate/axis/engineer/b_engineer2
{
	cull none
	implicitMap -
}

models/players/temperate/axis/cvops/b_cvops2
{
	cull none
	implicitMap -
}

models/players/temperate/axis/fieldops/b_fieldops2
{
	cull none
	implicitMap -
}

models/players/temperate/axis/medic/b_medic2
{
	cull none
	implicitMap -
}

models/players/temperate/axis/soldier/b_soldier2
{
	cull none
	implicitMap -
}


models/players/temperate/allied/soldier/body
{
	cull none
	implicitMap -
}

models/players/temperate/allied/soldier/b_soldier2
{
	cull none
	implicitMap -
}

models/players/temperate/allied/medic/b_medic
{
	cull none
	implicitMap -
}

models/players/temperate/allied/medic/b_medic2
{
	cull none
	implicitMap -
}

models/players/temperate/allied/engineer/b_engineer2
{
	cull none
	implicitMap -
}

models/players/temperate/allied/fieldops/b_fieldops
{
	cull none
	implicitMap -
}

models/players/temperate/allied/fieldops/b_fieldops2
{
	cull none
	implicitMap -
}

models/players/temperate/allied/cvops/b_cvops2
{
	cull none
	implicitMap -
}



// Reflective Effect

models/weapons2/c4_dynamite/dynamite_allied
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/c4_dynamite/alpha_dynamite_allied.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/c4_dynamite/dynamite_allied.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/multiplayer/medpack/first-aid
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/medpack/alpha_first-aid.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/multiplayer/medpack/first-aid.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/multiplayer/medpack/verbandkasten
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/medpack/alpha_verbandkasten.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/multiplayer/medpack/verbandkasten.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/multiplayer/smokegrenade/m18
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/smokegrenade/alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/multiplayer/smokegrenade/m18.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/multiplayer/smokegrenade/rauchsichtzeichen
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/smokegrenade/alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/multiplayer/smokegrenade/rauchsichtzeichen.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/multiplayer/smokebomb/an-m8
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/smokegrenade/alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/multiplayer/smokebomb/an-m8.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/multiplayer/smokebomb/nebelhandgranate-39
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/smokegrenade/alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/multiplayer/smokebomb/nebelhandgranate-39.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}



// Waving Grass by RayBan

models/foliage/grassfoliage1
{
	nopicmip
	qer_alphafunc greater 0.5
	qer_editorimage models/foliage/grassfoliage1.tga
	cull disable
	distanceCull 512 1792 0.49
	sort seethrough
	surfaceparm pointlight
	surfaceparm trans
	surfaceparm nomarks
	nopicmip
	deformVertexes wave 15 sin 0 1 0 0.25
	{
		map models/foliage/grassfoliage1.tga
		alphaFunc GE128
		rgbGen exactVertex
		alphaGen vertex
	}
}

models/foliage/grassfoliage2
{
	nopicmip
	qer_alphafunc greater 0.5
	qer_editorimage models/foliage/grassfoliage2.tga
	cull disable
	distanceCull 512 1536 0.49
	sort seethrough
	surfaceparm pointlight
	surfaceparm trans
	surfaceparm nomarks
	nopicmip
	deformVertexes wave 15 sin 0 1 0 0.25
	{
		map models/foliage/grassfoliage2.tga
		alphaFunc GE128
		rgbGen exactVertex
		alphaGen vertex
	}
}

models/foliage/grassfoliage3
{
	nopicmip
	qer_alphafunc greater 0.5
	qer_editorimage models/foliage/grassfoliage3.tga
	cull disable
	distanceCull 480 1280 0.49
	sort seethrough
	surfaceparm pointlight
	surfaceparm trans
	surfaceparm nomarks
	nopicmip
	deformVertexes wave 15 sin 0 1 0 0.25
	{
		map models/foliage/grassfoliage3.tga
		alphaFunc GE128
		rgbGen exactVertex
		alphaGen vertex
	}
}