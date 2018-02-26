#include <sponza/sponza.hpp>

using namespace sponza;

Material::Material(MaterialId i) : id(i)
{
}

MaterialId Material::getId() const
{
    return id;
}

Vector3 Material::getDiffuseColour() const
{
    return diffuse_colour;
}

void Material::setDiffuseColour(Vector3 c)
{
    diffuse_colour = c;
}

Vector3 Material::getSpecularColour() const
{
    return specular_colour;
}

void Material::setSpecularColour(Vector3 c)
{
    specular_colour = c;
}

float Material::getShininess() const
{
    return shininess;
}

void Material::setShininess(float s)
{
    shininess = s;
}

bool Material::isShiny() const
{
    return shininess > 0;
}
