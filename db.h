#include <iostream>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/collection.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <thread>
#include <vector>

mongocxx::instance inst{};
mongocxx::pool pool{mongocxx::uri{"mongodb://localhost:27017"}};

// Helper function to get a collection
mongocxx::collection getCollection(const std::string& db_name, const std::string& coll_name) {
    auto client = pool.acquire();
    return (*client)[db_name][coll_name];
}

int findDocumentWithLargestRootY(int root_x) {
    try {
        // Create a filter to find documents with root_x
        bsoncxx::builder::stream::document filter;
        filter << "root_x" << root_x;

        // Sort by root_y in descending order and limit to 1 document
        mongocxx::options::find options;
        options.sort(bsoncxx::builder::stream::document{} << "root_y" << -1 << bsoncxx::builder::stream::finalize)
               .limit(1);

        // Access the collection
        mongocxx::collection collection = getCollection("survey_cad", "num");

        // Execute the query
        auto cursor = collection.find(filter.view(), options);

        // If a document is found, return the largest root_y
        if (cursor.begin() != cursor.end()) {
            auto doc = *cursor.begin();
            int highest_root_y = doc["root_y"].get_int32().value;
            std::cout << "root_x = " << root_x << std::endl;
            return highest_root_y;
        } else {
            std::cout << "No document found with root_x = " << root_x << std::endl;
            return -1; // Return a default value indicating no results
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred during the query: " << e.what() << std::endl;
        return -1; // Return an error value
    }
}

void insertDocument(const std::string& data, int data_x, int data_y, int root_x, int root_y) {
    try {
        bsoncxx::builder::stream::document document{};

        document << "data" << data
                 << "data_x" << data_x
                 << "data_y" << data_y
                 << "root_x" << root_x
                 << "root_y" << root_y;

        // Get the collection and insert the document
        mongocxx::collection collection = getCollection("survey_cad", "num");
        collection.insert_one(document.view());

    } catch (const std::exception& e) {
        std::cerr << "An error occurred during insertion: " << e.what() << std::endl;
    }
}

void start_track(int root_x,int root_y){
    try {
        bsoncxx::builder::stream::document document{};

        document << "type" << "track"
                 << "root_x" << root_x
                 << "root_y" << root_y;

        // Get the collection and insert the document
        mongocxx::collection collection = getCollection("survey_cad", "num_track_det");
        collection.insert_one(document.view());

    } catch (const std::exception& e) {
        std::cerr << "An error occurred during insertion: " << e.what() << std::endl;
    }
}

int get_track(int root_x) {
    try {
        // Create a filter to find the document with the specified root_x
        bsoncxx::builder::stream::document filter;
        filter << "root_x" << root_x << "type" << "track";

        // Access the collection
        mongocxx::collection collection = getCollection("survey_cad", "num_track_det");

        // Execute the query
        auto cursor = collection.find(filter.view());

        // Retrieve the first matching document and return its root_y
        for (auto&& doc : cursor) {
            // Extract root_y from the document
            if (doc["root_y"]) {
                int root_y = doc["root_y"].get_int32().value;
                std::cout << "Document found: " << bsoncxx::to_json(doc) << std::endl;
                return root_y;
            }
        }

        // If no documents are found, return -1 to indicate not found
        std::cerr << "No document found with root_x = " << root_x << " and type = track" << std::endl;
        return -1;

    } catch (const std::exception& e) {
        std::cerr << "An error occurred while retrieving the document: " << e.what() << std::endl;
        return -1; // Return an error value
    }
}


void end_track(int root_x, int new_root_y) {
    try {
        // Create a filter to find the document with the specified root_x and type = "track"
        bsoncxx::builder::stream::document filter;
        filter << "root_x" << root_x << "type" << "track";

        // Create the update document to set the new root_y value
        bsoncxx::builder::stream::document update;
        update << "$set" << bsoncxx::builder::stream::open_document
               << "root_y" << new_root_y
               << bsoncxx::builder::stream::close_document;

        // Access the collection
        mongocxx::collection collection = getCollection("survey_cad", "num_track_det");

        // Check if the document exists in the collection
        auto cursor = collection.find(filter.view());
        bool document_found = false;

        for (auto&& doc : cursor) {
            document_found = true;
            std::cout << "Matching document found";
            break;
        }

        if (!document_found) {
            std::cout << "No document found with root_x = " << root_x << " and type = track." << std::endl;
            return;
        }

        // Perform the update operation
        auto result = collection.update_one(filter.view(), update.view());

        // Check if the document was updated
        if (result && result->modified_count() > 0) {
            std::cout << "Document with root_x = " << root_x << " successfully updated to new root_y = " << new_root_y << std::endl;
        } else {
            std::cout << "Document found, but no changes were made" << std::endl;
            // std::cout << "- The new root_y value is the same as the current value." << std::endl;
            // std::cout << "- The update operation did not execute correctly." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while updating the document: " << e.what() << std::endl;
    }
}
