#include <iostream>
#include <vector>
#include <string>
#include "httplib.h"
#include "json.hpp"

using namespace httplib;
using namespace std;
using json = nlohmann::json;

struct Task {
    int id;
    string title;
    string description;
    string status;
};

vector<Task> tasks;
int next_id = 1;

json task_to_json(const Task& t) {
    return json{
        {"id", t.id},
        {"title", t.title},
        {"description", t.description},
        {"status", t.status}
    };
}

int main() {
    Server server;

    // GET /tasks
    server.Get("/tasks", [](const Request&, Response& res) {
        json result = json::array();
        for (auto& t : tasks)
            result.push_back(task_to_json(t));

        res.set_content(result.dump(), "application/json");
        });

    // POST /tasks
    server.Post("/tasks", [](const Request& req, Response& res) {
        json body = json::parse(req.body);

        Task t;
        t.id = next_id++;
        t.title = body.value("title", "");
        t.description = body.value("description", "");
        t.status = body.value("status", "todo");

        tasks.push_back(t);

        res.status = 201;
        res.set_content(task_to_json(t).dump(), "application/json");
        });

    // GET /tasks/{id}
    server.Get(R"(/tasks/(\d+))", [](const Request& req, Response& res) {
        int id = stoi(req.matches[1]);

        for (auto& t : tasks) {
            if (t.id == id) {
                res.set_content(task_to_json(t).dump(), "application/json");
                return;
            }
        }

        res.status = 404;
        res.set_content("{\"error\":\"Not found\"}", "application/json");
        });

    // PUT /tasks/{id}
    server.Put(R"(/tasks/(\d+))", [](const Request& req, Response& res) {
        int id = stoi(req.matches[1]);
        json body = json::parse(req.body);

        for (auto& t : tasks) {
            if (t.id == id) {
                t.title = body.value("title", t.title);
                t.description = body.value("description", t.description);
                t.status = body.value("status", t.status);

                res.set_content(task_to_json(t).dump(), "application/json");
                return;
            }
        }

        res.status = 404;
        res.set_content("{\"error\":\"Not found\"}", "application/json");
        });

    // DELETE /tasks/{id}
    server.Delete(R"(/tasks/(\d+))", [](const Request& req, Response& res) {
        int id = stoi(req.matches[1]);

        for (size_t i = 0; i < tasks.size(); i++) {
            if (tasks[i].id == id) {
                tasks.erase(tasks.begin() + i);
                res.set_content("{\"result\":\"deleted\"}", "application/json");
                return;
            }
        }

        res.status = 404;
        res.set_content("{\"error\":\"Not found\"}", "application/json");
        });

    cout << "Server started on http://localhost:8080\n";
    server.listen("0.0.0.0", 8080);
}
