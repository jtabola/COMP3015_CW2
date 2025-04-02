# OpenGL Shader Project Report

## User Interaction and Executable Usage

### How to Open and Control the Software

#### Opening the Executable
1. Open the solution folder (COMP3015 CW2)
2. Navigate to the "**x64**" folder.
3. Navigate to the "**Release**" folder containing `Project_Template.exe`.
4. Double-click `Project_Template.exe` to launch the application.

#### User Controls
- **Pause Rotation:** Hold `Spacebar` to stop object rotation.
- **Quit:** Press `Esc` to exit the application.

---

## Code Structure and Functionality

### Overview of Classes and Functions

#### `SceneBasic_Uniform` (Main Scene Class)
- Handles rendering, shader management, lighting, particles and transformations.
- **`initScene()`** - Initializes OpenGL, loads models, and textures.
- **`update(float t)`** - Updates animation and physics.
- **`render()`** - Renders the scene and particles.
- **`resize(int w, int h)`** - Adjusts viewport and projection matrix.
- **`initParticles()`** - Initializes particles.
- **`updateParticles(float deltaTime)`** - Updates particle positions and life.
- **`renderParticles()`** - Renders particles with blending.

#### `globals.h` Header
The globals header stores **`stopRotation`** **bool** which gets toggled, using the **toggleRotation** function whenever the **`scenerunner.h`** loop receives user input.

### Shaders Used
- **`basic_uniform.vert/.frag`** - Main rendering shaders that handles normal and diffuse texture blending as well as light and fog calculations.
- **`particle.vert/.frag`** - Handles fire particle effects and color blending of particle texture with particle life color gradient.
- **`finalComposition.vert/.frag`** - Merges bloom and scene rendering.
  
---

## Shader Program Uniqueness and Comparisons

### Concept and Inspiration
- The idea started with a basic OpenGL scene featuring a textured 3D skull.
- Expanded to include fire particles and a bloom effect for a visually striking appearance.
- Inspired by real-time rendering techniques in modern game engines.

### Uniqueness and Enhancements
- Uses a Gaussian blur-based bloom to enhance fire brightness.
- Customisable shader settings (e.g., light colors, particle speed).
- Optimised particle rendering using instancing to improve performance.

---

## Video Report Link
Watch the detailed explanation here: **[Insert actual video link]**

### Additional Topics in Video:
- Deep dive into shader functions.
- Performance considerations (balancing visual quality and FPS).
- Reflection on development process and potential improvements.

---

## Evaluation

### Achievements
- Successfully implemented an interactive OpenGL rendering pipeline.  
- Real-time particle effects and bloom for enhanced visual effect.
- Implemented multiple shaders that work seamlessly

### Improvements
- Improve particle physics (e.g., wind, turbulence effects, gravity).
- Add more control options (e.g., skull movement, camera rotate).
- Implement a **geometry shader** to generate more complex particle effects dynamically.
