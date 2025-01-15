std::string base64_encode(const std::vector<uchar>& data) {

    if (data.empty()) {
    throw std::invalid_argument("Input data for base64 encoding is empty.");
}

    static const char s_encoding[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    std::string encoded;
    int val = 0, valb = -6;
    for (uchar c : data) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(s_encoding[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) encoded.push_back(s_encoding[((val << 8) >> valb) & 0x3F]);
    while (encoded.size() % 4) encoded.push_back('=');
    return encoded;
}

std::string image_to_data_url(const cv::Mat& image) {
    if (image.empty()) {
        throw std::invalid_argument("Image is empty or not loaded correctly.");
    }

    // Encode the image to PNG format
    std::vector<uchar> buffer;
    std::vector<int> params = { cv::IMWRITE_PNG_COMPRESSION, 9 }; // PNG compression level
    cv::imencode(".png", image, buffer, params);

    if (buffer.empty()) {
    throw std::runtime_error("Failed to encode image to PNG format.");
}

    // Encode buffer to base64
    std::string base64_data = base64_encode(buffer);

    // Create Data URL
    base64_data = "data:image/png;base64," + base64_data;

    return base64_data;
}

void prt(std::string str){
    std::cout << str << std::endl;
}


bool isSimilarToPurple(const cv::Vec3b &pixel)
{
    int blue = pixel[0];
    int green = pixel[1];
    int red = pixel[2];

    // Define the range for purple (you may need to tweak these values based on your needs)
    int lower_bound = 50; // Lower bound for blue and red
    int upper_bound = 255; // Upper bound for blue and red
    int green_max = 50;   // Max value for green to avoid green dominance

    return (blue >= lower_bound && blue <= upper_bound) &&
           (red >= lower_bound && red <= upper_bound) &&
           (green <= green_max);
}
