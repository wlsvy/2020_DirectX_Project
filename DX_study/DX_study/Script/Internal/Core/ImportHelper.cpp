#include "ImportHelper.h"
#include "FileImporter.h"

#include <DirectXMath.h>

bool Importer::LoadModel(const std::string & filePath)
{
	ModelImporter importer;
	return importer.LoadModel(filePath);
}
