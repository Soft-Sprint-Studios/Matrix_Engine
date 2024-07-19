# PMF format

Pathos uses material scripts not unlike Source's for defining textures 
used by the world and models. A PMF file(short for Pathos Material File)
has two main types:
 
# Alias scripts:
This type of script is used to tell the engine that it refers to another
material file in another location. A typical alias script will look like
the one below:

>$alias <br />
>{ <br />
>	$scriptfile models/civilians/civilian_body1.pmf <br />
>} <br />

This is useful for textures that are shared across models.

# Texture scripts:
Texture scripts store the base definitions of textures. This is used to
define how textures behave and what properties they have. Here is an
example with alpha testing enabled, and with a luminance map added:

>$texture <br />
>{ <br />
>	$alphatest <br />
>	$texture diffuse models/shellcasings/pistolcasing_silver.tga <br />
>	$texture luminance models/bullets/pistolcasing_silver_glow.tga <br />
>} <br />

Below described are the behaviors of each individual option in the PMF
script format:

# Textures:
Currently five types of textures are supported:
 - diffuse : This is the base color texture.
 - normalmap/normal : This is the texture containing normal values.
 - detail : The detail texture used. See $dt_scalex and $dt_scaley.
 - specular : This defined the strength of specular light reflections
              for the texture, and also cubemap reflections. See
			  $cubemaps, $phong_exp, $spec, $cubemapstrength.
 - luminance : This is used for textures that have a self-illumination
               component to them.
 - ao : This is used for Ambient Occlusion textures, same as PBR   

# Flags and values:
 - $cubemaps : Marks that the texture will use cubemap reflections.
 - $fullbright : The texture is fullbright, and will not be affected by 
                 any lights.
 - $nodecal : This texture will not have any decals applied to it.
 - $alphatest : The texture will have transparent pixels removed.
 - $nomipmaps : Mip-maps will not be generated for this texture.
 - $clamp : The textue will be clamped to the 0-1 texcoord range.
 - $eyeglint : This texture will have the eye glint texture applied
               to it with spheremapping. This only works for models.
 - $chrome : A chrome effect will be applied to this texture. Only
			 works for transparent world brushes, but works for any
			 model textures.
 - $additive : The texture will be rendered with additive blending.
               Only works for models.
 - $alphablend : The texture will be rendered using alpha blending.
                 Only works for models.
 - $scope : The texture will have a copy of the framebuffer rendered
            as part of it, and the alpha component of the model
			texture will be added with alpha blending onto it.
 - $noimpactfx : Impact effects will not be spawned on this texture
                 if it is hit by a weapon.
 - $nostepsound : Footstep sounds will not play for the player or
                  NPCs if they are walking on this texture.
 - $nofacecull : Disables backface culling.
 - $nopenetration : Bullets cannot penetrated through this texture.
 - $bulletproof : Bullets will become lodged inside this brush.
 - $dt_scalex : Detail texture x scale.
 - $dt_scaley : Detail texture y scale.
 - $int_width : Currently unused, added for a custom level editor
                that doesn't rely on WAD in the future.
 - $int_height : Currently unused, added for a custom level editor
                that doesn't rely on WAD in the future.
 - $alpha : Specifies the alpha value of the texture, used for
            the $alphablend option.
 - $phong_exp : Phong exponent multiplier value. Only works if you
                are using specular maps.
 - $spec : Specular value multiplier. Only works if you are using
           a specular map.
 - $scopescale : Defines the FOV value the scope effect is scaled
                 down to. This should match what your weapon will
				 use as it's zoomed-in FOV value.
 - $cubemapstrength : Defines the strength of the cubemap effect's
                      reflections, with a value between 0 and 1.
 - $cubemapnormal : Defines the strength of normal mapping on the cubemap
 - $container : Used by textures stored in WAD3 files, and should
                specify the name of the WAD3 file.
 - $scrollu : Defines the scroll speed on the U coordinate.
 - $scrollv : Defines the scroll speed on the V coordinate.
 - $fullbrightcolor : Defines the fullbright color for this
                      texture. Only works with models for now.
 - $material : Defines the material type for this texture, used
               for impact effects and footstep sounds.