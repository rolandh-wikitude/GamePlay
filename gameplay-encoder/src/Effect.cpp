#include "Base.h"
#include "Effect.h"
#include "Light.h"
#include "Scene.h"
#include "Node.h"
#include <copyfile.h>

namespace gameplay
{
std::string Effect::wrapStr[] = { "REPEAT", "CLAMP" };
std::string Effect::filterStr[] = {
    "NEAREST",
    "LINEAR",
    "NEAREST_MIPMAP_NEAREST",
    "LINEAR_MIPMAP_NEAREST",
    "NEAREST_MIPMAP_LINEAR",
    "LINEAR_MIPMAP_LINEAR"
};

Effect::Effect(void) :
    ambientColor(Vector4(.2f, .2f, .2f, 1.0f)),
    diffuseColor(Vector4(.8f, .8f, .8f, 1.0f)),
    specularColor(Vector4(.0f, .0f, .0f, .0f)),
    specularExponent(.0f),
    wrapS(REPEAT),
    wrapT(REPEAT),
    minFilter(NEAREST_MIPMAP_LINEAR),
    magFilter(LINEAR),
    texFilename(""),
    hasTexture(false),
    alpha(1.0f),
    texAbsPath(""),
    texDestinationPath("")
{

}

Effect::~Effect(void)
{
}

unsigned int Effect::getTypeId(void) const
{
    return EFFECT_ID;
}

const char* Effect::getElementName(void) const
{
    return "Effect";
}

void Effect::writeBinary(FILE* file)
{
    Object::writeBinary(file);
    write(_vertexShader, file);
    write(_fragmentShader, file);
}

void Effect::writeText(FILE* file)
{
}

void Effect::writeEffect(FILE* file, Light* light)
{
    copyTexture();
    std::string unlit = "";
    if(light == NULL)
    {
        unlit = "-unlit";
    }
    else
    {
        fprintf(file, "\t\t\tdefines = SPECULAR");
        unsigned char lt = light->getLightType();
        if (lt == Light::PointLight)
        {
            fprintf(file, ";POINT_LIGHT");
        }
        else if (lt == Light::SpotLight)
        {
            fprintf(file, ";SPOT_LIGHT");
        }
//        else
//        {
//            // Light::DirectionalLight (already defined in shader)
//        }

//        // this will be set in the sceneviewer
//        fprintf(file, "\n\t\t\tu_ambientColor = %f, %f, %f, %f\n",
//                this->ambientColor.x,
//                this->ambientColor.y,
//                this->ambientColor.z,
//                this->ambientColor.w);
//

        // TEST
//        fprintf(file, "\n\t\t\tu_lightColor = %f, %f, %f, 1.0\n",
//                this->specularColor.x,
//                this->specularColor.y,
//                this->specularColor.z);

//        if (!light->isAmbient())
//        {
//            fprintf(file, "\t\t\tu_lightColor = %f, %f, %f\n",
//                    light->getRed(),
//                    light->getGreen(),
//                    light->getBlue());
//        }

        fprintf(file, "\n\t\t\tu_specularExponent = %f\n", this->specularExponent);
        // fprintf(file, "\t\t\tu_modulateAlpha = %f\n\n", this->alpha);
    }

    if(this->hasTexture)
    {
        fprintf(file, "\t\t\tvertexShader = shaders/textured%s.vert\n", unlit.c_str());
        fprintf(file, "\t\t\tfragmentShader = shaders/textured%s.frag\n\n", unlit.c_str());
        fprintf(file, "\t\t\tsampler u_diffuseTexture\n");
        fprintf(file, "\t\t\t{\n");
        // e.g. path = duck-diffuse.png
        fprintf(file, "\t\t\t\tpath = %s\n", this->texFilename.c_str());
        fprintf(file, "\t\t\t\twrapS = %s\n", wrapStr[this->wrapS].c_str());
        fprintf(file, "\t\t\t\twrapT = %s\n", wrapStr[this->wrapT].c_str());
        // TODO: set proper mipmap-values
        // fprintf(file, "\t\t\t\tminFilter = %s\n", filterStr[this->minFilter].c_str());
        // fprintf(file, "\t\t\t\tmagFilter = %s\n", filterStr[this->magFilter].c_str());
        fprintf(file, "\t\t\t\tminFilter = LINEAR\n");
        fprintf(file, "\t\t\t\tmagFilter = LINEAR\n");
        fprintf(file, "\t\t\t\tmipmap = false\n");
        fprintf(file, "\t\t\t}\n\n");
    }
    else
    {
        fprintf(file, "\t\t\tvertexShader = shaders/colored%s.vert\n", unlit.c_str());
        fprintf(file, "\t\t\tfragmentShader = shaders/colored%s.frag\n\n", unlit.c_str());
            fprintf(file, "\t\t\tu_diffuseColor = %f, %f, %f, %f\n",
            this->diffuseColor.x,
            this->diffuseColor.y,
            this->diffuseColor.z,
            this->diffuseColor.w);
    }
    fprintf(file, "\t\t\trenderState\n");
    fprintf(file, "\t\t\t{\n");
    fprintf(file, "\t\t\t\tcullFace = false\n");
    fprintf(file, "\t\t\t\tdepthTest = true\n");
    fprintf(file, "\t\t\t}\n");
}

void Effect::setAmbient(Vector4 color)
{
    this->ambientColor = color;
}

void Effect::setDiffuse(Vector4 color)
{
    this->diffuseColor = color;
}

void Effect::setSpecular(Vector4 color)
{
    this->specularColor = color;
}

void Effect::setShininess(float shininess)
{
    this->specularExponent = shininess;
}

void Effect::setWrapS(Wrap wrapS)
{
    this->wrapS = wrapS;
}

void Effect::setWrapT(Wrap wrapT)
{
    this->wrapT = wrapT;
}

void Effect::setMinFilter(Filter minFilter)
{
    this->minFilter = minFilter;
}

void Effect::setMagFilter(Filter magFilter)
{
    this->magFilter = magFilter;
}

void Effect::setTextureFilename(std::string path, std::string gpbOutputPath)
{
    int x = path.find_last_of('/');
    this->texFilename = "tex/" + path.substr(path.find_last_of('/') + 1);
    //this->texFilename = gpbOutputPath + "/tex/" + path.substr(path.find_last_of('/') + 1);
    this->hasTexture = true;
}

void Effect::setTextureFilePath(std::string path, std::string gpbOutputPath)
{
    int index = path.find("file://");
    if (index != std::string::npos)
    {
        path = path.substr(7);
    }
    if(path[0] == '.')
    {
        path = gpbOutputPath + path.substr(path.find_first_of('.') + 1);
    }
    this->texAbsPath = path;
}

std::string Effect::getTextureFilePath()
{
    return this->texAbsPath;
}

void Effect::setAlpha(float alpha)
{
    this->alpha = alpha;
}

void Effect::setTexDestinationPath(std::string texDestinationPath)
{
    size_t pos = this->texAbsPath.find_last_of('/');
    if(pos != std::string::npos) {
        this->texDestinationPath = texDestinationPath + this->texAbsPath.substr(pos);
    }
    else {
        this->texDestinationPath = texDestinationPath + this->texAbsPath;
    }
}

void Effect::copyTexture()
{
    if (!this->texDestinationPath.empty() && this->texAbsPath.compare(this->texDestinationPath) != 0)
    {
        int result = copyfile(this->texAbsPath.c_str(), this->texDestinationPath.c_str(), NULL, COPYFILE_DATA);
    }
}
}
