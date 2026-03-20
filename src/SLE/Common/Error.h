#pragma once

#include <iosfwd>
#include <iostream>
#include <streambuf>

#include <cstdio>


std::ostream& Err();

class DefaultErrorStreamBuf : public std::streambuf
{
public:

	DefaultErrorStreamBuf();
	~DefaultErrorStreamBuf() override;

private:

	int overflow(int _Character) override;

	int sync() override;
};