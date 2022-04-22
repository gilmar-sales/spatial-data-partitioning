void init_particles(Particle* particles) {
    int half_width = Application::get()->get_window().get_width() / 2;
    int half_height = Application::get()->get_window().get_height() / 2;

    long* seed = new long();
    auto generator = std::default_random_engine((long) seed);
    std::uniform_int_distribution<int> x_distribution(-(half_width-max_radius),half_width-max_radius);
    std::uniform_int_distribution<int> y_distribution(-(half_height-max_radius), half_height-max_radius);
    std::uniform_int_distribution<int> velocity_distribution(10, 20);
    std::uniform_int_distribution<int> color_distribution(25, 100);
    std::uniform_int_distribution<int> radius_distribution(min_radius, max_radius);

    for (unsigned i = 0; i < particles_count; i++)
    {
        Particle& particle = particles[i];
        particle.position.x = (float) x_distribution(generator);
        particle.position.y = (float) y_distribution(generator);

        particle.velocity = (particle.position / glm::length(particle.position)) * (float) velocity_distribution(generator);

        particle.radius = (float) radius_distribution(generator);

        particle.color.x = (float) color_distribution(generator) / 100;
        particle.color.y = (float) color_distribution(generator) / 100;
        particle.color.z = (float) color_distribution(generator) / 100;
    }
}

unsigned initCircle(glm::vec2 point, float radius) {
    std::vector<glm::vec2> vertices = std::vector<glm::vec2>(32);

    vertices[0] = point;


    for (unsigned i = 1; i < 32; i++) {
        float angle = (360.f/30.f) * (float) (i - 1);
        glm::vec2 vertice = {glm::cos(glm::radians(angle)), glm::sin(glm::radians(angle))};
        vertices[i] = vertice;
    }

    std::vector<unsigned > indices = std::vector<unsigned >(96);

    for(unsigned i = 0; i < 32; i++) {
        indices[0+(i*3)] = 0;
        indices[1+(i*3)] = i+1;
        indices[2+(i*3)] = i+2;
    }

    //indices[89] = 1;

    unsigned VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])* vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}


unsigned initQuad(glm::vec2 point) {

    std::vector<glm::vec2> vertices = {
            {-1,1},
            {1,1},
            {1, -1},
            {-1,-1}
    };


    std::vector<unsigned > indices = {
            0, 1,
            1, 2,
            2,3,
            3,0
    };


    unsigned VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])* vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}
