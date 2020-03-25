// NoQuarter Shaders

// arty warning
gfx/hud/noquarter/saber
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_arty.tga
		blendFunc blend
		rgbGen vertex
	}
}

//Nade Warning Icons
gfx/hud/noquarter/masher
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_grenade.tga
		blendFunc blend
		rgbGen vertex
	}
}

gfx/hud/noquarter/pineapple
{
	nomipmaps
	nopicmip
	{
		map icons/iconw_pineapple.tga
		blendFunc blend
		rgbGen vertex
	}
}

//Regenerate Icon
sprites/regenerate
{
	nocompress
	nopicmip
	{
		map gfx/hud/noquarter/regenerate.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

//Poison Syringe
models/multiplayer/syringe/poison
{
	cull disable
	{
		map gfx/misc/poisonfluid.tga
		rgbGen lightingDiffuse
		tcMod scale 2 2
		tcMod scroll 0 -0.2
		tcMod turb 0 0.4 0 0.1
		//tcMod rotate 0.5
	}
	{
		map gfx/misc/poisonfluid.tga
		blendfunc blend
		rgbGen lightingDiffuse
		tcMod scale 2 1.7
		tcMod scroll 0 -0.21
		tcMod turb 0 0.1 0 0.1
		//tcMod rotate -0.6
	}
	{
		map gfx/misc/poisonfluid.tga
		blendfunc blend
		rgbGen lightingDiffuse
		tcMod scale 1.5 2.5
		tcMod scroll 0 -0.21
		tcMod turb 0 0.35 0 0.1
		//tcMod rotate 0.4
	}
	{
		map models/multiplayer/syringe/syringePoison.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
	{
		map models/multiplayer/syringe/syringe_reflections.tga
		blendfunc blend
		rgbGen lightingDiffuse
	}
}

//Obituary Icons
gfx/hud/goombaObituary
{
	nocompress
	nopicmip
	{
		map gfx/hud/goombaObituary.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/fallObituary
{
	nocompress
	nopicmip
	{
		map gfx/hud/goombaObituary.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/drownObituary
{
	nocompress
	nopicmip
	{
		map gfx/2d/waterhint.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/artyObituary
{
	nocompress
	nopicmip
	{
		map icons/iconw_binoculars.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/airstrikeObituary
{
	nocompress
	nopicmip
	{
		map icons/iconw_smokegrenade.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/poisonObituary
{
	nocompress
	nopicmip
	{
		map gfx/hud/poisonObituary.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/shoveObituary
{
	nocompress
	nopicmip
	{
		map gfx/hud/shoveObituary.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/crushObituary
{
	nocompress
	nopicmip
	{
		map gfx/hud/artyObituary.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// Poison Overlay
gfx/misc/poisonoverlay
{
	{
		map gfx/misc/poisonoverlay.tga
		blendFunc blend
		rgbGen vertex
		tcMod stretch sin 1.5 0.2 0 0.2
		tcMod rotate 50
		tcMod turb 1.5 0.15 2 0.7
		tcMod scroll 0.22 0.1
		alphaGen wave sin 0.5 0.2 0 1		
	}
	{
		map gfx/misc/poisonoverlay.tga
		blendFunc blend
		rgbGen vertex
		tcMod stretch sin 1.8 0.2 0.8 0.2
		tcMod rotate 10
		tcMod turb 1.2 0.2 0 0.5
		tcMod scroll -.56 0.4
		alphaGen wave sin 0.5 0.3 0.85 0.15
	}
}

// Poisoned Icon
sprites/poisoned
{
	nocompress
	nopicmip
	{
		map sprites/poisoned.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

// Goldrush bank floor shader was missing, this gives a nice reflection.
textures/egypt_floor_sd/marblefloor_c04
{
	{
		map textures/goldrush-env.tga
		rgbGen identity
		tcGen environment
		tcMod scale 0.9 0.9
	}
	{
		map textures/egypt_floor_sd/marblefloor_c04.tga
		blendfunc blend
		rgbGen identity
	}
	{
		map $lightmap
		blendfunc filter
		rgbGen identity
	}
}

//Helmet Culling fix
models/players/temperate/allied/soldier/helmet
{
	cull none
	implicitMap -
}

models/players/temperate/allied/medic/helmet
{
	cull none
	implicitMap -
}

models/players/temperate/allied/engineer/helmet
{
	cull none
	implicitMap -
}

models/players/temperate/allied/fieldops/helmet
{
	cull none
	implicitMap -
}

models/players/temperate/allied/covertops/helmet
{
	cull none
	implicitMap -
}

//Axis Helmet Armor
models/players/temperate/axis/armor
{
	cull disable 
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/players/temperate/axis/armor.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}


//Missle Cam Grain Shader
gfx/misc/tv_grain
{
	{
		map gfx/misc/lines.tga
		blendfunc gl_dst_color gl_one_minus_src_alpha
		rgbGen identity
		tcMod turb 0.8 0.7 0.39 1
	}
	{
		animmap 15 gfx/misc/grain.tga gfx/misc/grain2.tga gfx/misc/grain3.tga gfx/misc/grain4.tga 
		blendfunc gl_dst_color gl_src_alpha
	}
	{
		map gfx/misc/overlay.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//Shell Casing Shaders
models/weapons2/shells/venom_shell
{
	{
		map textures/effects/envmap_gold.tga
		rgbGen identity
		tcGen environment
	}
	{
		map models/weapons2/shells/venom_shell.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen lightingdiffuse
	}
}

models/weapons2/shells/3006_shell
{
	{
		map textures/effects/envmap_gold.tga
		rgbGen identity
		tcGen environment
	}
	{
		map models/weapons2/shells/3006_shell.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen lightingdiffuse
	}
}

models/weapons2/shells/9mm_shell
{
	{
		map textures/effects/envmap_gold.tga
		rgbGen identity
		tcGen environment
	}
	{
		map models/weapons2/shells/9mm_shell.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen lightingdiffuse
	}
}

//Ammopack Shader Fix
models/multiplayer/supplies/ammobox
{
	cull disable 
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/supplies/ammobox.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/multiplayer/supplies/ammobox_2
{
	cull disable 
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/supplies/ammobox_2.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/multiplayer/supplies/ammobox_3
{
	cull disable 
	{
		map textures/effects/envmap_slate_90.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/supplies/ammobox_3.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//Sleeve and Hand Shader fixes
/*
models/weapons2/arms/arm_allied
{	
	cull none
	implicitMap -
}

models/weapons2/arms/arm_axis
{	
	cull none
	implicitMap -
}
*/

models/weapons2/mp40/hand16
{	
	cull none
	implicitMap -
}

//Sunset Background
ui/assets/sunset
{
	nopicmip
	{
		map ui/assets/sunset.tga
		tcMod stretch sin 1.15 0.125 0 0.01
	}
	{
		clampmap ui/assets/background_mask.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen identity
		tcMod stretch sin 1.1 0.05 0 0.01
	}
}

//Logos
ui/assets/logo_ss_dark
{
	nomipmaps
	nopicmip
	{
		clampmap ui/assets/logo_ss.tga
		blendfunc blend
		alphaGen const 0.25
	}
}

//Hints
sprites/resuscitate
{
	nomipmaps
	nopicmip
	{
		map sprites/reviveActivateHint.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/2d/ammoboxHint
{
	nomipmaps
	nopicmip
	{
		map gfx/2d/ammobox64.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/2d/healthHint
{
	nomipmaps
	nopicmip
	{
		map gfx/2d/healthCross.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}


ui/assets/logo_nq_dark
{
	nomipmaps
	nopicmip
	{
		clampmap ui/assets/logo_nq.tga
		blendfunc blend
		alphaGen const 0.25
	}
}

//Disguised Shader
sprites/disguised
{
	nocompress
	nopicmip
	{
		map sprites/undercover.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex	
	}
}

//Naked Shader Fix
models/players/temperate/common/bare_legs
{
	cull none
	implicitMap -
}

models/players/temperate/common/bare_legs_allied
{
	cull none
	implicitMap -
}

models/players/temperate/common/bare_legs_2
{
	cull none
	implicitMap -
}

models/players/temperate/common/bare_legs_2_allied
{
	cull none
	implicitMap -
}

models/players/temperate/common/bare_legs_3
{
	cull none
	implicitMap -
}

models/players/temperate/common/bare_legs_3_allied
{
	cull none
	implicitMap -
}

//Silencer Shader Fix
models/weapons2/luger/silncer
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/luger/silncer.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

//Rocket Flare Fix
models/ammo/rocket/rockflar
{
	cull disable
	{
		map models/ammo/rocket/rockflar.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
	}
}

models/ammo/rocket/rockfls2
{
	cull disable
	{
		map models/ammo/rocket/rockfls2.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
	}
}

// Venom Shaders
models/weapons2/venom/v4
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/venom/venom.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/weapons2/venom/venom_brass
{
	{
		map textures/effects/envmap_gold.tga
		rgbGen identity
		tcGen environment
	}
	{
		map models/weapons2/venom/venom.tga
		blendFunc GL_DST_COLOR GL_SRC_COLOR
		rgbGen lightingdiffuse
	}
}

// Bazooka Shaders
models/weapons2/bazooka/bazooka
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/bazooka/bazooka_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/bazooka/bazooka.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/weapons2/bazooka/bazookaTube
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/bazooka/bazookaTube_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/bazooka/bazookaTube.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

//Axis Granatwefer 34
models/multiplayer/mortar/mortar_ax
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/mortar/mortar_ax.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/multiplayer/mortar/mortar_shell_ax
{
	{
		map models/multiplayer/mortar/mortar_shell_ax.jpg
		rgbGen lightingdiffuse
	}
}

// Winchester Shaders
models/weapons2/winchester/shotgunBarrel
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/winchester/shotgunBarrel_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/winchester/shotgunBarrel.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/weapons2/winchester/shotgunStock
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/winchester/shotgunStock_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/winchester/shotgunStock.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/weapons2/winchester/shotgunCooler
{
	cull none
	{
		map models/weapons2/winchester/shotgunCooler.tga
		rgbGen const ( 0 0 0 ) 
		alphaFunc GE128
		depthWrite
	}
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
		depthFunc equal
	}
	{
		map models/weapons2/winchester/shotgunCooler.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
		depthFunc equal
	}
}

// KA-BAR Knife Shaders
models/weapons2/knife_kbar/knife_yd
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/knife_kbar/knife_yd_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/knife_kbar/knife_yd.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

//BAR Shaders
models/weapons2/bar/barStock
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{	
		map models/weapons2/bar/barStock_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/bar/barStock.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/weapons2/bar/barClipBolt
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{	
		map models/weapons2/bar/barClipBolt_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/bar/barClipBolt.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/weapons2/bar/barSight
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{	
		map models/weapons2/bar/barSight_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/bar/barSight.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/weapons2/bar/barMisc
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{	
		map models/weapons2/bar/barMisc_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/bar/barMisc.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

models/weapons2/bar/barBarrel
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{	
		map models/weapons2/bar/barBarrel_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/bar/barBarrel.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}


//Mobile Browning .30 cal MG
models/weapons2/browning/browning
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/multiplayer/browning/browning.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/weapons2/browning/biped
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{
		map models/weapons2/browning/biped.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/weapons2/browning/barrel
{
	cull none
	{
		map models/multiplayer/browning/barrel.tga
		rgbGen const ( 0 0 0 ) 
		alphaFunc GE128
		depthWrite
	}
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
		depthFunc equal
	}
	{
		map models/multiplayer/browning/barrel.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
		depthFunc equal
	}
}

//StG44 Shaders
models/weapons2/StG44/mp44stock
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{	
		map models/weapons2/StG44/mp44stock_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/StG44/mp44stock.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}

}

models/weapons2/StG44/mp44body
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingdiffuse
		tcGen environment
	}
	{	
		map models/weapons2/StG44/mp44body_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/StG44/mp44body.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

//MP34 shader
models/weapons2/mp34/mp34
{
	cull disable
	{
		map models/weapons2/mp34/mp34.tga
		rgbGen const ( 0 0 0 )
		alphaFunc GE128
	}
	{
		map models/weapons2/mp34/mp34.tga
		blendFunc GL_ONE GL_ONE_MINUS_SRC_ALPHA
		rgbGen lightingdiffuse
	}
}

models/weapons2/mp34/mp34stock
{
	{
		map textures/effects/envmap_slate.tga
		rgbGen lightingDiffuse
		tcGen environment 
	}
	{	
		map models/weapons2/mp34/mp34stock_alpha.jpg
		blendFunc GL_DST_COLOR GL_ZERO
		rgbGen identity
	}
	{
		map models/weapons2/mp34/mp34stock.jpg
		blendFunc GL_ONE GL_ONE
		rgbGen lightingdiffuse
	}
}

//No Quarter HUD Shaders
gfx/hud/noquarter/fastCool
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/fast-cool.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/2d/pistolShotColt
{
	nomipmaps
	nopicmip
	{
		map gfx/2d/pistolShot.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/2d/pistolShotLuger
{
	nomipmaps
	nopicmip
	{
		map gfx/2d/pistol-luger.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/adrenaline
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/adrenaline.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/binoculars
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/binoculars.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/caduceus
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/caduceus64.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/2d/reviveActivateHint
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/caduceus.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
		tcMod scale 1 -1
	}
}

gfx/hud/noquarter/helmet_allies
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/helmet_allies.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/helmet_axis
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/helmet_axis.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/morale_allies
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/morale_allies.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/morale_axis
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/morale_axis.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/flakjacket_allies
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/flakjacket_allies.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}

gfx/hud/noquarter/flakjacket_axis
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/noquarter/flakjacket_axis.tga
		blendFunc GL_SRC_ALPHA GL_ONE_MINUS_SRC_ALPHA
		rgbGen vertex
	}
}


//HUD Ranks//
gfx/hud/ranks/rank1
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank1.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank2
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank2.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank3
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank3.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank4
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank4.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank5
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank5.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank6
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank6.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank7
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank7.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank8
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank8.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank9
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank9.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank10
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank10.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank11
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank11.tga
		blendfunc blend
		rgbGen vertex
	}
}
gfx/hud/ranks/rank12
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank12.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank13
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank13.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank14
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank14.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank15
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank15.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank16
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank16.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank17
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank17.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank18
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank18.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/rank19
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/rank19.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank1
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank1.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank2
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank2.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank3
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank3.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank4
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank4.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank5
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank5.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank6
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank6.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank7
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank7.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank8
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank8.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank9
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank9.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank10
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank10.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank11
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank11.tga
		blendfunc blend
		rgbGen vertex
	}
}
gfx/hud/ranks/xrank12
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank12.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank13
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank13.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank14
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank14.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank15
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank15.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank16
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank16.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank17
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank17.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank18
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank18.tga
		blendfunc blend
		rgbGen vertex
	}
}

gfx/hud/ranks/xrank19
{
	nomipmaps
	nopicmip
	{
		map gfx/hud/ranks/xrank19.tga
		blendfunc blend
		rgbGen vertex
	}
}
