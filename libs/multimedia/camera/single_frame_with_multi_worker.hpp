#ifndef OCV_SINGLE_FRAME_WITH_MULTI_WORKER_HPP_3094580394827
#define OCV_SINGLE_FRAME_WITH_MULTI_WORKER_HPP_3094580394827

#include <QObject>

#include <any>
#include <memory>

namespace ocv{

class frame_capture_controller;
class frame_capture_opencv_worker;
class frame_process_controller;
class frame_process_base_worker;

struct frame_capture_params;

class single_frame_with_multi_worker : public QObject
{
    Q_OBJECT
public:
    explicit single_frame_with_multi_worker(frame_capture_params const &params,
                                            QObject *parent = nullptr);
    ~single_frame_with_multi_worker();

    void add_listener(std::shared_ptr<frame_process_controller> process_controller, void *key);
    void remove_listener(void *key);    

signals:
    void message_error(QString msg);
    void send_process_results(std::any results);

private:
    void stop();

    std::unique_ptr<frame_capture_controller> frame_capture_controller_;
    std::vector<std::pair<std::shared_ptr<frame_process_controller>, void*>> frame_process_vector_;
};

}

#endif // SINGLE_FRAME_WITH_MULTI_WORKER_HPP
