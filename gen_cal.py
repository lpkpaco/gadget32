from PIL import Image, ImageDraw, ImageFont
def clashes(start, end, cal):
    for name, (s, e) in cal.items():
        if start < e and s < end:
            return name
    return None

def add_event(cal, name, start, end):
    if end <= start:
        return False, "end must be after start"
    hit = clashes(start, end, cal)
    if hit:
        return False, f"clashes with {hit}"
    cal[name] = [start, end]
    items = sorted(cal.items(), key=lambda kv: (kv[1][0], kv[1][1]))
    cal.clear()
    cal.update(items)
    return True, "added"

def draw_calendar(cal_dict):
    cal = Image.new('RGB', (240, 240), color='white')
    d = ImageDraw.Draw(cal)
    if len(cal_dict) <= 3:
        for i in range(0, len(cal_dict)):
            key = list(cal_dict.keys())[i]
            value = list(cal_dict.values())[i]
            rect_start_y = (i)*(180/len(cal_dict)) + 5
            rect_end_y = (i+1)*(180/len(cal_dict)) - 5
            event_start_y = rect_start_y + 10
            time_start_y = event_start_y + 15
            d.rectangle([10, rect_start_y, 230, rect_end_y], outline=(0, 214, 255, 153), width=2, fill=(0, 214, 255, 94))
            d.text([20, event_start_y], key, fill=(0, 0, 0), align="left", font=ImageFont.truetype("arial.ttf", 12))
            d.text([20, time_start_y], str(value[0]) + " - " + str(value[1]), fill=(0, 0, 0), align="left", font=ImageFont.truetype("arial.ttf", 12))
            i += 1
    cal.save("calendar.png")

if __name__ == "__main__":
    cal_dict = {"CCST9042": [1300, 1500], "CCCH9023": [1500, 1700]}
    event = input()
    time1 = int(input())
    time2 = int(input())
    print(add_event(cal_dict, event, time1, time2))
    draw_calendar(cal_dict)