/*
 * $FU-Copyright$
 */

namespace ARA { 
   /**
    * This class provides the cubic evaporation function for an ant 
	* routing algorithm.
    */
    class CubicEvaporationPolicy {
        public:
            CubicEvaporationPolicy(float plateau, float center, int degree);
            float evaporate(float oldPheromoneValue, float milliSecondsSinceLastTraffic);

        private:
            float plateau;
            float center;
            int degree;
    };
} 

