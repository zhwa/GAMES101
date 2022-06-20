#include <iostream>
#include <opencv2/opencv.hpp>
#include "rasterizer.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "OBJ_Loader.h"
#include <numbers>
#include <iostream>

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f translate{ {1.F, 0, 0, -eye_pos[0]}, {0, 1.F, 0, -eye_pos[1]}, {0, 0, 1.F, -eye_pos[2]}, {0, 0, 0, 1.F} };
    return translate;
}

Eigen::Matrix4f get_model_matrix(float angle)
{
    const float rad = angle * std::numbers::pi_v<float> / 180.F;
    const Eigen::Matrix4f rotation{ {std::cos(rad), 0, std::sin(rad), 0}, {0, 1.F, 0, 0}, {-std::sin(rad), 0, std::cos(rad), 0}, {0, 0, 0, 1.F} };
    const Eigen::Matrix4f scale{ {2.5F, 0, 0, 0}, {0, 2.5F, 0, 0}, {0, 0, 2.5F, 0}, {0, 0, 0, 1.F} };
    const Eigen::Matrix4f translate{ {1.F, 0, 0, 0}, {0, 1.F, 0, 0}, {0, 0, 1.F, 0}, {0, 0, 0, 1.F} };
    return translate * rotation * scale;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar)
{
    // Create the projection matrix for the given parameters.
    // Then return it.

    // Note: the equaiton says t = abs(zNear) * tanf(eye_fov / 2), but in that way the image would be vertically inversed. So, use the negative value here just as a dirty workaround.
    float t = -std::abs(zNear) * std::tanf(eye_fov / 2);
    float r = t * aspect_ratio;
    float l = -r;
    float b = -t;

    // Orthographic Projection.
    Eigen::Matrix4f Mtrans{ {2.F / (r - l), 0, 0, 0}, {0, 2.F / (t - b), 0, 0}, {0, 0, 2.F / (zNear - zFar), 0}, {0, 0, 0, 1.F} };
    Eigen::Matrix4f Mscale{ {1.F, 0, 0, -0.5F * (r + l)}, {0, 1.F, 0, -0.5F * (t + b)}, {0, 0, 1.F, -0.5F * (zNear + zFar)}, {0, 0, 0, 1.F} };
    Eigen::Matrix4f Mor = Mtrans * Mscale;

    // M persp->ortho
    Eigen::Matrix4f Mpo{ {zNear, 0, 0, 0}, {0, zNear, 0, 0}, {0, 0, zNear + zFar, -zNear * zFar}, {0, 0, 1.F, 0} };

    // Perspective Projection.
    Eigen::Matrix4f Mpersp = Mor * Mpo;

    return Mpersp;
}

Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f;
    return { return_color.x() * 255, return_color.y() * 255, return_color.z() * 255  };
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

struct light
{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        const auto& text_coordinates = payload.tex_coords;
        return_color = payload.texture->getColor(text_coordinates.x(), text_coordinates.y());
    }
    Eigen::Vector3f texture_color{ return_color.x(), return_color.y(), return_color.z() };

    const Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    const Eigen::Vector3f kd = texture_color / 255.f;
    const Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    const auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    const auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    const std::vector<light> lights = {l1, l2};
    const Eigen::Vector3f amb_light_intensity{10, 10, 10};
    const Eigen::Vector3f eye_pos{0, 0, 10};

    const float p = 150.F;

    const Eigen::Vector3f point = payload.view_pos;
    const Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        // Ambient: Ka * Ia
        result_color += ka.cwiseProduct(amb_light_intensity);
        // Diffuse: kd * (I/r^2) * max(0, n*l)
        const Eigen::Vector3f l = light.position - point;
        const Eigen::Vector3f ln = l.normalized();
        const float r2 = 1.F / l.squaredNorm();
        const float cosine_nl = normal.dot(ln);
        result_color += kd.cwiseProduct(light.intensity) * r2 * std::max(0.F, cosine_nl);
        // Specular: ks * (I/r^2) * max(0, n*h)^p
        const auto h = (eye_pos.normalized() + ln).normalized();
        const float cosine_nh = normal.dot(h);
        result_color += ks.cwiseProduct(light.intensity) * r2 * std::powf(std::max(0.F, cosine_nh), p);
    }

    return result_color * 255.F;
}

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    const Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    const Eigen::Vector3f kd = payload.color;
    const Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    const auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    const auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    const std::vector<light> lights = {l1, l2};
    const Eigen::Vector3f amb_light_intensity{10, 10, 10};
    const Eigen::Vector3f eye_pos{0, 0, 10};

    const float p = 150;

    const Eigen::Vector3f point = payload.view_pos;
    const Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};
    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        // Ambient: Ka * Ia
        result_color += ka.cwiseProduct(amb_light_intensity);
        // Diffuse: kd * (I/r^2) * max(0, n*l)
        const Eigen::Vector3f l = light.position - point;
        const Eigen::Vector3f ln = l.normalized();
        const float r2 = 1.F / l.squaredNorm();
        const float cosine_nl = normal.dot(ln);
        result_color += kd.cwiseProduct(light.intensity) * r2 * std::max(0.F, cosine_nl);
        // Specular: ks * (I/r^2) * max(0, n*h)^p
        const auto h = (eye_pos.normalized() + ln).normalized();
        const float cosine_nh = normal.dot(h);
        result_color += ks.cwiseProduct(light.intensity) * r2 * std::powf(std::max(0.F, cosine_nh), p);
    }
    return result_color * 255.F;
}

Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2F, kn = 0.1F;
    
    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    Eigen::Vector3f n{ normal };
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    float xz2 = std::sqrt(n.x() * n.x() + n.z() * n.z());
    Eigen::Vector3f t{ n.x() * n.y() / xz2, xz2, n.z() * n.y() / xz2 };
    // Vector b = n cross product t
    Eigen::Vector3f b = n.cross(t);
    // Matrix TBN = [t b n]
    Eigen::Matrix3f TBN{ {t.x(), b.x(), n.x()}, {t.y(), b.y(), n.y()}, {t.z(), b.z(), n.z()} };
    // Get texture.
    const auto& text_coord = payload.tex_coords;
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    const float delta_w = 1.F / static_cast<float>(payload.texture->width);
    const float delta_v = 1.F / static_cast<float>(payload.texture->height);
    const float dU = kh * kn * (payload.texture->getColor(text_coord.x() + delta_w, text_coord.y()) - payload.texture->getColor(text_coord.x(), text_coord.y())).x();
    const float dV = kh * kn * (payload.texture->getColor(text_coord.x(), text_coord.y() + delta_v) - payload.texture->getColor(text_coord.x(), text_coord.y())).y();
    // Vector ln = (-dU, -dV, 1)
    Eigen::Vector3f ln{ -dU, -dV, 1 };
    // Position p = p + kn * n * h(u,v)
    point += 0.15 * n.cwiseProduct(payload.texture->getColor(text_coord.x(), text_coord.y()));
    // Normal n = normalize(TBN * ln)
    normal = (TBN * ln).normalized();

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        // Ambient: Ka * Ia
        result_color += ka.cwiseProduct(amb_light_intensity);
        // Diffuse: kd * (I/r^2) * max(0, n*l)
        const Eigen::Vector3f l = light.position - point;
        const Eigen::Vector3f lnormal = l.normalized();
        const float r2 = 1.F / l.squaredNorm();
        const float cosine_nl = normal.dot(lnormal);
        result_color += kd.cwiseProduct(light.intensity) * r2 * std::max(0.F, cosine_nl);
        // Specular: ks * (I/r^2) * max(0, n*h)^p
        const auto h = (eye_pos.normalized() + lnormal).normalized();
        const float cosine_nh = normal.dot(h);
        result_color += ks.cwiseProduct(light.intensity) * r2 * std::powf(std::max(0.F, cosine_nh), p);
    }

    return result_color * 255.f;
}

Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f normal = payload.normal;
    float kh = 0.2F, kn = 0.1F;

    // TODO: Implement bump mapping here
    // Let n = normal = (x, y, z)
    Eigen::Vector3f n{ normal };
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    float xz2 = std::sqrt(n.x() * n.x() + n.z() * n.z());
    Eigen::Vector3f t{ n.x() * n.y() / xz2, xz2, n.z() * n.y() / xz2 };
    // Vector b = n cross product t
    Eigen::Vector3f b = n.cross(t);
    // Matrix TBN = [t b n]
    Eigen::Matrix3f TBN{ {t.x(), b.x(), n.x()}, {t.y(), b.y(), n.y()}, {t.z(), b.z(), n.z()} };
    // Get texture.
    const auto& text_coord = payload.tex_coords;
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    const float delta_w = 1.F / static_cast<float>(payload.texture->width);
    const float delta_v = 1.F / static_cast<float>(payload.texture->height);
    const float dU = kh * kn * (payload.texture->getColor(text_coord.x() + delta_w, text_coord.y()) - payload.texture->getColor(text_coord.x(), text_coord.y())).x();
    const float dV = kh * kn * (payload.texture->getColor(text_coord.x(), text_coord.y() + delta_v) - payload.texture->getColor(text_coord.x(), text_coord.y())).y();
    // Vector ln = (-dU, -dV, 1)
    Eigen::Vector3f ln{ -dU, -dV, 1 };
    // Normal n = normalize(TBN * ln)
    normal = (TBN * ln).normalized();

    return normal * 255.f;
}

int main(int argc, const char** argv)
{
    std::vector<Triangle*> TriangleList;

    float angle = 140.0;
    bool command_line = false;

    std::string filename = "output.png";
    objl::Loader Loader;
    std::string obj_path = "models/spot/";

    // Load .obj File
    bool loadout = Loader.LoadFile("models/spot/spot_triangulated_good.obj");
    for (const auto& mesh : Loader.LoadedMeshes)
    {
        for (size_t i=0; i<mesh.Vertices.size(); i+=3)
        {
            Triangle* t = new Triangle();
            for (size_t j=0; j<3; j++)
            {
                t->setVertex(j, Eigen::Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0));
                t->setNormal(j, Eigen::Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z));
                t->setTexCoord(j, Eigen::Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));
            }
            TriangleList.emplace_back(t);
        }
    }

    rst::rasterizer r(700, 700);

    auto texture_path = "hmap.jpg";
    r.set_texture(Texture(obj_path + texture_path));

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = normal_fragment_shader;

    if (argc >= 2)
    {
        command_line = true;
        filename = std::string(argv[1]);

        if (argc == 3 && std::string(argv[2]) == "texture")
        {
            std::cout << "Rasterizing using the texture shader\n";
            active_shader = texture_fragment_shader;
            texture_path = "spot_texture.png";
            r.set_texture(Texture(obj_path + texture_path));
        }
        else if (argc == 3 && std::string(argv[2]) == "normal")
        {
            std::cout << "Rasterizing using the normal shader\n";
            active_shader = normal_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "phong")
        {
            std::cout << "Rasterizing using the phong shader\n";
            active_shader = phong_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "bump")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = bump_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "displacement")
        {
            std::cout << "Rasterizing using the displacement shader\n";
            active_shader = displacement_fragment_shader;
        }
    }

    Eigen::Vector3f eye_pos = {0,0,10};

    r.set_vertex_shader(vertex_shader);
    r.set_fragment_shader(active_shader);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        cv::imwrite(filename, image);
        key = cv::waitKey(10);

        if (key == 'a' )
        {
            angle -= 0.1;
        }
        else if (key == 'd')
        {
            angle += 0.1;
        }

    }
    return 0;
}