#include "FLModel2D.hpp"

FLModel2D::FLModel2D(FLDevice& device, std::vector<Vertex> vertices, std::vector<uint16_t> indices)
	: device(device), vertices(vertices), indices(indices) {

}

FLModel2D::~FLModel2D(){

}
