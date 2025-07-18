using Grasshopper.Kernel.Types;
using Rhino.Display;
using Rhino.DocObjects;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PumaGrasshopper
{
    class Materials { 
        public static Material CreateMaterial(bool isPBR, Color diffuse, Color ambient, Color specular, double opacity, double shininess, Color4f emission, double metallic, double roughness)
        {
            Material mat = new Material()
            {
                DiffuseColor = diffuse,
                AmbientColor = ambient,
                SpecularColor = specular,
                Transparency = 1.0 - opacity,
                Shine = shininess,
                FresnelReflections = true,
            };

            if(isPBR)
            {
                mat.ToPhysicallyBased();

                mat.PhysicallyBased.Emission = emission;
                mat.PhysicallyBased.Metallic = metallic;
                mat.PhysicallyBased.Roughness = roughness;
                mat.PhysicallyBased.Opacity = opacity;
                mat.PhysicallyBased.BaseColor = new Color4f(diffuse);
            }

            mat.CommitChanges();

            return mat;
        }
    }
}
