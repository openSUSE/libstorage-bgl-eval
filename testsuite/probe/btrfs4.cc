
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/UsedFeatures.h"

#include "testsuite/helpers/TsCmp.h"


using namespace std;
using namespace storage;


/**
 * The devicegraph in this test has 1. parallel holders and 2. a cycle.
 */
BOOST_AUTO_TEST_CASE(probe)
{
    setenv("YAST_BTRFS_SNAPSHOT_RELATIONS", "yes", 1);

    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("btrfs4-mockup.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();
    probed->check();

    Devicegraph* staging = storage.get_staging();
    staging->load("btrfs4-devicegraph.xml");
    staging->check();

    TsCmpDevicegraph cmp(*probed, *staging);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_BITWISE_EQUAL(probed->used_features(), (uint64_t)(UF_BTRFS));
}