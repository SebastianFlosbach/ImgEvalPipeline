// AlembicReader.cpp : Defines the entry point for the application.
//

#include "AlembicReader.h"

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcGeom/All.h>

#include <boost/program_options.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>

using namespace Alembic::Abc;
using namespace Alembic;
namespace po = boost::program_options;

int indentation = -1;
void printObject(const IObject& obj);
void printProperties(const ICompoundProperty& prop);

int main(int argc, const char* argv[])
{
	std::string sfmFile;
	std::string outputFile;

	po::options_description params("Params");
	params.add_options()
		("help,h", "Produce help message.")
		("input,i", po::value<std::string>(&sfmFile)->required(), "SfMData file.")
		("output,o", po::value<std::string>(&outputFile)->required(), "Output file for poses.");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, params), vm);

	if (vm.count("help") || argc == 1) {
		std::cout << params << std::endl;
		return EXIT_SUCCESS;
	}

	vm.notify();

	std::ofstream outfile;
	outfile.open(outputFile, std::ofstream::out | std::ofstream::trunc);

	Alembic::Abc::IArchive archive(Alembic::AbcCoreOgawa::ReadArchive(), sfmFile, Alembic::Abc::ErrorHandler::kThrowPolicy);
	Alembic::Abc::IObject archiveTop = archive.getTop();

	IObject root(archiveTop, "mvgRoot");
	IObject cameras(root, "mvgCameras");

	size_t numCameras = cameras.getNumChildren();
	for (size_t i = 0; i < numCameras; i++)
	{
		IObject camera = cameras.getChild(i);
		if (AbcGeom::IXformSchema::matches(camera.getMetaData())) {
			auto schema = AbcGeom::IXform(camera).getSchema();
			AbcGeom::XformSample sample;
			schema.get(sample);
			auto m = sample.getMatrix();
			auto mT = m.transposed();

			double mT00 = mT.x[0][0];
			double mT10 = mT.x[1][0];
			double mT20 = mT.x[2][0];
			double mT01 = mT.x[0][1];
			double mT11 = mT.x[1][1];
			double mT21 = mT.x[2][1];
			double mT02 = mT.x[0][2];
			double mT12 = mT.x[1][2];
			double mT22 = mT.x[2][2];
			double mT03 = mT.x[0][3];
			double mT13 = mT.x[1][3];
			double mT23 = mT.x[2][3];

			Imath::V3d translation = { mT03, mT13, mT23 };

			mT.x[0][3] = 0;
			mT.x[1][3] = 0;
			mT.x[2][3] = 0;

			//Imath::V3d sx = { mT.x[0][0],mT.x[1][0] ,mT.x[2][0] };
			//Imath::V3d sy = { mT.x[0][1],mT.x[1][1] ,mT.x[2][1] };
			//Imath::V3d sz = { mT.x[0][2],mT.x[1][2] ,mT.x[2][2] };

			//Imath::V3d scale = { sx.length(), sy.length(), sz.length() };

			double qw, qx, qy, qz;

			double tr = mT00 + mT11 + mT22;
			if (tr > 0) {
				double S = sqrt(tr + 1.0) * 2;
				qw = 0.25 * S;
				qx = (mT21 - mT12) / S;
				qy = (mT02 - mT20) / S;
				qz = (mT10 - mT01) / S;
			}
			else if ((mT00 > mT11) & (mT00 > mT22)) {
				float S = sqrt(1.0 + mT00 - mT11 - mT22) * 2; // S=4*qx 
				qw = (mT21 - mT12) / S;
				qx = 0.25 * S;
				qy = (mT01 + mT10) / S;
				qz = (mT02 + mT20) / S;
			}
			else if (mT11 > mT22) {
				float S = sqrt(1.0 + mT11 - mT00 - mT22) * 2; // S=4*qy
				qw = (mT02 - mT20) / S;
				qx = (mT01 + mT10) / S;
				qy = 0.25 * S;
				qz = (mT12 + mT21) / S;
			}
			else {
				float S = sqrt(1.0 + mT22 - mT00 - mT11) * 2; // S=4*qz
				qw = (mT10 - mT01) / S;
				qx = (mT02 + mT20) / S;
				qy = (mT12 + mT21) / S;
				qz = 0.25 * S;
			}


			auto name = camera.getName();
			std::string delim = "_";
			auto start = 0U;
			auto end = name.find(delim);
			std::vector<std::string> splits;
			while (end != std::string::npos)
			{
				splits.push_back(name.substr(start, end - start));
				start = end + delim.length();
				end = name.find(delim, start);
			}

			std::string poseName;
			for (size_t i = 3; i < splits.size(); i++)
			{
				poseName.append(splits[i]);
				if (i != splits.size() - 1) {
					poseName.append("_");
				}
			}

			outfile << poseName << std::endl;
			outfile << qw << " " << qx << " " << qy << " " << qz << std::endl;
			outfile << translation[0] << " " << translation[1] << " " << translation[2] << std::endl;
		}
	}

	//printObject(root);

	return 0;
}

void printObject(const IObject& obj) {
	++indentation;
	std::cout << std::string(indentation, '\t') << "Name: " << obj.getName() << std::endl;
	std::cout << std::string(indentation, '\t') << "NumChilds: " << obj.getNumChildren() << std::endl;

	ICompoundProperty props = obj.getProperties();
	printProperties(props);

	size_t numChildren = obj.getNumChildren();
	for (int ii = 0; ii < numChildren; ++ii)
	{
		printObject(obj.getChild(ii));
	}
	--indentation;
}

void printProperties(const ICompoundProperty& prop) {
	size_t numProperties = prop.getNumProperties();
	if (numProperties == 0) {
		std::cout << std::string(indentation, '\t') << "No Properties" << std::endl;
	}

	for (int pp = 0; pp < numProperties; pp++)
	{
		PropertyType pType = prop.getPropertyHeader(pp).getPropertyType();
		std::cout << std::string(indentation, '\t') << "PropertyName: " << prop.getPropertyHeader(pp).getName();
		if (pType == kCompoundProperty)
		{
			std::cout << " (compound)" << std::endl;
		}
		else if (pType == kScalarProperty)
		{
			std::cout << " (scalar)" << std::endl;
		}
		else if (pType == kArrayProperty)
		{
			std::cout << "(array)" << std::endl;
		}
	}
}
