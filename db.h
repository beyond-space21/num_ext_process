#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/options/find.hpp>
#include <string>

class MongoDBClient {
public:
    // Constructor to initialize the client with a unique connection
    MongoDBClient(const std::string& uri = "mongodb://localhost:27017") 
        : client(mongocxx::uri{uri}) {}

    // Function to access a collection
    mongocxx::collection getCollection(const std::string& db_name, const std::string& coll_name) {
        return client[db_name][coll_name];
    }

    // Function to find the document with the largest root_y for a given root_x
    int findDocumentWithLargestRootY(int root_x) {
        try {
            bsoncxx::builder::stream::document filter;
            filter << "root_x" << root_x;

            mongocxx::options::find options;
            options.sort(bsoncxx::builder::stream::document{} << "root_y" << -1 << bsoncxx::builder::stream::finalize)
                   .limit(1);

            mongocxx::collection collection = getCollection("survey_cad", "num");
            auto cursor = collection.find(filter.view(), options);

            if (cursor.begin() != cursor.end()) {
                auto doc = *cursor.begin();
                int highest_root_y = doc["root_y"].get_int32().value;
                return highest_root_y;
            }
            return -1; // No document found
        } catch (const std::exception& e) {
            std::cerr << "Error finding document: " << e.what() << std::endl;
            return -1;
        }
    }

    // Function to insert a document
    void insertDocument(const std::string& data, int data_x, int data_y, int root_x, int root_y) {
        try {
            bsoncxx::builder::stream::document document{};
            document << "data" << data
                     << "data_x" << data_x
                     << "data_y" << data_y
                     << "root_x" << root_x
                     << "root_y" << root_y;

            mongocxx::collection collection = getCollection("survey_cad", "num");
            collection.insert_one(document.view());
        } catch (const std::exception& e) {
            std::cerr << "Error inserting document: " << e.what() << std::endl;
        }
    }

    // Function to manage tracking documents
    void startTrack(int root_x, int root_y) {
        try {
            bsoncxx::builder::stream::document document{};
            document << "type" << "track"
                     << "root_x" << root_x
                     << "root_y" << root_y;

            mongocxx::collection collection = getCollection("survey_cad", "num_track_det");
            collection.insert_one(document.view());
        } catch (const std::exception& e) {
            std::cerr << "Error starting track: " << e.what() << std::endl;
        }
    }

    int getTrack(int root_x) {
        try {
            bsoncxx::builder::stream::document filter;
            filter << "root_x" << root_x << "type" << "track";

            mongocxx::collection collection = getCollection("survey_cad", "num_track_det");
            auto cursor = collection.find(filter.view());

            for (auto&& doc : cursor) {
                if (doc["root_y"]) {
                    return doc["root_y"].get_int32().value;
                }
            }
            return -1; // Not found
        } catch (const std::exception& e) {
            std::cerr << "Error getting track: " << e.what() << std::endl;
            return -1;
        }
    }

    void endTrack(int root_x, int new_root_y) {
        try {
            bsoncxx::builder::stream::document filter;
            filter << "root_x" << root_x << "type" << "track";

            bsoncxx::builder::stream::document update;
            update << "$set" << bsoncxx::builder::stream::open_document
                   << "root_y" << new_root_y
                   << bsoncxx::builder::stream::close_document;

            mongocxx::collection collection = getCollection("survey_cad", "num_track_det");
            auto result = collection.update_one(filter.view(), update.view());

            if (!result || result->modified_count() == 0) {
                std::cerr << "Track not updated: root_x = " << root_x << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error ending track: " << e.what() << std::endl;
        }
    }

private:
    mongocxx::client client;
};

// int main() {
//     // Example usage of the MongoDBClient class
//     MongoDBClient client;

//     // Insert a document
//     client.insertDocument("test_data", 10, 20, 1, 100);

//     // Find the document with the largest root_y
//     int max_root_y = client.findDocumentWithLargestRootY(1);
//     std::cout << "Max root_y: " << max_root_y << std::endl;

//     // Start tracking
//     client.startTrack(1, 100);

//     // Get track
//     int track_root_y = client.getTrack(1);
//     std::cout << "Track root_y: " << track_root_y << std::endl;

//     // End tracking
//     client.endTrack(1, 200);

//     return 0;
// }
