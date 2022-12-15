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
    void test_alloc_write_then_read(void);
    void test_free(void);
};

/************************* FUNCTION ************************/
void TestAllocateOnGPU::testMethodsRegistration (void)
{
    setTestCaseName("TestAllocateOnGPU");
    SVUT_REG_TEST_METHOD(test_alloc_write_then_read);
    SVUT_REG_TEST_METHOD(test_free);

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
void TestAllocateOnGPU::test_alloc_write_then_read(void)
{
    // initialize host arrays
    int N = 100;
    int a[N];
    int check_a[N];

    for (int i = 0; i < N; ++i) {
        a[i] = i;
        check_a[i] = 0;
    }

    // allocate on gpu
    void * d_a = sctk_malloc_on_gpu(N* sizeof(int));

    // WRITE : copy data to device buffer
    CHECK_ERROR(cuMemcpyHtoD(CUdeviceptr(d_a), a, N * sizeof(int)));

    // READ BACK : copy back into another host buffer
    CHECK_ERROR(cuMemcpyDtoH(check_a, CUdeviceptr(d_a), N * sizeof(int)));

    // check incoming data
    for (int i = 0; i < N; ++i) {
        SVUT_ASSERT_EQUAL(a[i], check_a[i]);
    }
}

void TestAllocateOnGPU::test_free()
{
    // allocate on gpu
    void * d_a = sctk_malloc_on_gpu(100 * sizeof(int));

    // deallocate
    d_a = sctk_free_on_gpu (CUdeviceptr(d_a));

    SVUT_ASSERT_EQUAL(d_a, nullptr);
}

SVUT_REGISTER_STANDELONE(TestAllocateOnGPU);