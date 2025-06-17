#ifndef servos
#define servos

/**
 * @brief Sets the platform to its default position.
 * 
 * This function moves the servos controlling the platform to their default 
 * positions, which are set to 90 degrees.
 */
void plattform_default_position(void);

/**
 * @brief Empties the platform.
 * 
 * This function moves the servo controlling the platform to a position 
 * that empties its contents. A delay of 500 ms is added before the movement.
 */
void plattform_empty(void);

/**
 * @brief Empties the platform in the red direction.
 * 
 * This function moves the servo controlling the platform to the 45-degree 
 * position (red direction) and then calls `plattform_empty()` to complete 
 * the emptying process.
 */
void plattform_empty_r(void);

/**
 * @brief Empties the platform in the green direction.
 * 
 * This function moves the servo controlling the platform to the 90-degree 
 * position (green direction) and then calls `plattform_empty()` to complete 
 * the emptying process.
 */
void plattform_empty_g(void);

/**
 * @brief Empties the platform in the blue direction.
 * 
 * This function moves the servo controlling the platform to the 135-degree 
 * position (blue direction) and then calls `plattform_empty()` to complete 
 * the emptying process.
 */
void plattform_empty_b(void);

#endif