#include <boost/test/unit_test.hpp>

#include "Bullseye/Algo_Manager.hpp"

BOOST_AUTO_TEST_CASE (Algo_Manager_Construction_Test)
{
  dart::Algo_Manager engine;
  if (engine.activate () == -1) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT ("cannot start decision engine")));
  }
  engine.wait ();
}
