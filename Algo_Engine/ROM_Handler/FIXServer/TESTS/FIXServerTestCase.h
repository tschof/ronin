#ifndef FIXSERVER_TEST_CASE_H
#define FIXSERVER_TEST_CASE_H

#include "CPPTest\TestCase.h"
#include "..\FIXServer.h"

class FIXServerTestCase : public CPPTest::TestCase<FIXServer>
{
	public:
		FIXServerTestCase();

	private:
		typedef CPPTest::EmptyTest<FIXServer> EmptyTest;
};

#endif
