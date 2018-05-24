/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package it.imati.cnr.tools;

/**
 *
 * @author daniela
 */
public class PrintingParameters {
        
    public double layer_thickness = 40.0;
    double laser_beam_diameter = 0.0;
    
    double chamber_dim_width = 0.0;
    double chamber_dim_height = 0.0;
    double chamber_dim_depth = 0.0;
    
    double no_print_zone_1_x = 0.0;
    double no_print_zone_1_y = 0.0;
    double no_print_zone_1_z = 0.0;
    
    double no_print_zone_2_x = 0.0;
    double no_print_zone_2_y = 0.0;
    double no_print_zone_2_z = 0.0;
    
    double chamber_temperature = 0.0;
    double material_density = 0.0;
    
    double thermal_expansion_coefficient = 0.0;
    double thermal_shrinkage = 0.0;
    
    double young_modulus = 0.0;
    double elastic_modulus_at_infinite_time = 0.0;
    double poisson_ratio = 0.0;
    
    double elastic_viscosity = 0.0;
    double yield_stress = 0.0;
    
    double saturation_stress = 0.0;
    
    String isotropic_hardening_low = "";
    double linear_isotropic_hardening_coefficient = 0.0;
    double exp_isotropic_hardening_coefficient = 0.0;
    
    double kinematic_hardening = 0.0;
    double plastic_viscosity = 0.0;
    double critical_temperature = 0.0;
        
    String material = "Inconel 718";
}
