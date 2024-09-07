// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class MarchingCubesTable
{
public:

	// Constructor
	MarchingCubesTable();

	// Edge and Triangle lookup tables
	static const int EDGE_TABLE[256];
	static const int TRI_TABLE[256][16];
	static const int EDGE_VERTICES[12][2];
};

