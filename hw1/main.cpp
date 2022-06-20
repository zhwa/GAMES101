#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f translate{ {1.F, 0, 0, -eye_pos[0]}, { 0, 1.F, 0, -eye_pos[1] }, { 0, 0, 1.F, -eye_pos[2] }, { 0, 0, 0, 1.F } };
    return translate;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    // Note: no need to use Rodrigues' Rotation Formula.
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    Eigen::Matrix4f model{ {std::cos(rotation_angle), -std::sin(rotation_angle), 0, 0}, {std::sin(rotation_angle), std::cos(rotation_angle), 0, 0}, {0, 0, 1.F, 0}, {0, 0, 0, 1.F} };
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Create the projection matrix for the given parameters.
    // Then return it.

    float t = std::abs(zNear) * std::tanf(eye_fov / 2);
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

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";

    if (argc >= 3)
    {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc == 4)
        {
            filename = std::string(argv[3]);
        }
        else
        {
            return 0;
        }
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 5};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}