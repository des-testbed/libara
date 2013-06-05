/*
 * $FU-Copyright$
 */

#include <cmath>
#include <cstdlib>
#include <stdint.h>

#include "CubicEvaporationPolicy.h"

using namespace ARA;

CubicEvaporationPolicy::CubicEvaporationPolicy(float plateau, float center, int degree){
    this->plateau = plateau;
    this->center = center;
    this->degree = degree;
}

float CubicEvaporationPolicy::evaporate(float oldPheromoneValue, float milliSecondsSinceLastTraffic) {
        float alpha = pow(center,degree);
        float evaporation = (plateau/alpha)*(alpha+pow((milliSecondsSinceLastTraffic-center),degree));
        float newPheromoneValue = oldPheromoneValue-evaporation;

        if(newPheromoneValue>=0.0) {
            return(newPheromoneValue);
        } else {
            return 0;
        }
}
