/* ############################# MPC License ############################## */
/* # Wed Nov 19 15:19:19 CET 2008                                         # */
/* # Copyright or (C) or Copr. Commissariat a l'Energie Atomique          # */
/* #                                                                      # */
/* # IDDN.FR.001.230040.000.S.P.2007.000.10000                            # */
/* # This file is part of the MPC Runtime.                                # */
/* #                                                                      # */
/* # This software is governed by the CeCILL-C license under French law   # */
/* # and abiding by the rules of distribution of free software.  You can  # */
/* # use, modify and/ or redistribute the software under the terms of     # */
/* # the CeCILL-C license as circulated by CEA, CNRS and INRIA at the     # */
/* # following URL http://www.cecill.info.                                # */
/* #                                                                      # */
/* # The fact that you are presently reading this means that you have     # */
/* # had knowledge of the CeCILL-C license and that you accept its        # */
/* # terms.                                                               # */
/* #                                                                      # */
/* # Authors:                                                             # */
/* #   - Valat Sébastien sebastien.valat@cea.fr                           # */
/* #   - Adam Julien julien.adam@cea.fr                                   # */
/* #                                                                      # */
/* ######################################################################## */

/************************** HEADERS ************************/
#include <svUnitTest/svUnitTest.h>
#include <unistd.h>
#include <cstring>

#include <sctk_allocator.h>
#include "test_helper.h"

/************************** USING **************************/
using namespace svUnitTest;

/*************************** CLASS *************************/
class TestAllocateOnGPU : public svutTestCase
{
public:
    void testMethodsRegistration(void);
    virtual void setUp(void);
    virtual void tearDown(void);
protected:
    void test_somewhere_to_start(void);
};

/************************* FUNCTION ************************/
void TestAllocateOnGPU::testMethodsRegistration (void)
{
    setTestCaseName("TestAllocateOnGPU");
    SVUT_REG_TEST_METHOD(test_somewhere_to_start);
}

/************************* FUNCTION ************************/
void TestAllocateOnGPU::setUp (void)
{
}

/************************* FUNCTION ************************/
void TestAllocateOnGPU::tearDown (void)
{
}


/************************* FUNCTION ************************/
void TestAllocateOnGPU::test_somewhere_to_start(void )
{
    sctk_malloc_on_gpu(12);
    SVUT_ASSERT_TODO("add some checkings");
}

SVUT_REGISTER_STANDELONE(TestAllocateOnGPU);
