from PIL import Image, ImageDraw, ImageFont
def clashes(start, end, cal):
    for name, (s, e) in cal.items():
        if start < e and s < end:
            return name
    return None

def add_event(cal, name, start, end, isEmpty):
    if isEmpty:
        cal.update({name: [start, end]})
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
    if len(cal_dict) == 1:
            for i in range(0, len(cal_dict)):
                key = list(cal_dict.keys())[i]
                value = list(cal_dict.values())[i]
                rect_start_y = 5
                rect_end_y = 85
                event_start_y = rect_start_y + 10
                time_start_y = event_start_y + 20
                d.rectangle([10, rect_start_y, 230, rect_end_y], outline=(0, 214, 255, 153), width=2, fill=(0, 214, 255, 94))
                d.text([20, event_start_y], key, fill=(255, 255, 255), align="left", font=ImageFont.truetype("arial.ttf", 18))
                d.text([20, time_start_y], str(value[0]) + " - " + str(value[1]), fill=(255, 255, 255), align="left", font=ImageFont.truetype("arial.ttf", 18))
            i += 1
    elif 1 <len(cal_dict) <= 3:
        for i in range(0, len(cal_dict)):
            key = list(cal_dict.keys())[i]
            value = list(cal_dict.values())[i]
            rect_start_y = (i)*(180/len(cal_dict)) + 5
            rect_end_y = (i+1)*(180/len(cal_dict)) - 5
            event_start_y = rect_start_y + 6
            time_start_y = event_start_y + 17
            d.rectangle([10, rect_start_y, 230, rect_end_y], outline=(0, 214, 255, 153), width=2, fill=(0, 214, 255, 94))
            d.text([20, event_start_y], key, fill=(255, 255, 255), align="left", font=ImageFont.truetype("arial.ttf", 18))
            d.text([20, time_start_y], str(value[0]) + " - " + str(value[1]), fill=(255, 255, 255), align="left", font=ImageFont.truetype("arial.ttf", 18))
            i += 1
    else:
        for i in range(0, len(cal_dict)):
            key = list(cal_dict.keys())[i]
            value = list(cal_dict.values())[i]
            rect_start_y = (i)*(240/len(cal_dict)) + 3
            rect_end_y = (i+1)*(240/len(cal_dict)) - 3
            event_start_y = rect_start_y + 5
            time_start_y = event_start_y + 19 - len(cal_dict)
            d.rectangle([10, rect_start_y, 230, rect_end_y], outline=(0, 214, 255, 153), width=2, fill=(0, 214, 255, 94))
            d.text([20, event_start_y], key, fill=(255, 255, 255), align="left", font=ImageFont.truetype("arial.ttf", 20-len(cal_dict)))
            d.text([20, time_start_y], str(value[0]) + " - " + str(value[1]), fill=(255, 255, 255), align="left", font=ImageFont.truetype("arial.ttf", 20-len(cal_dict)))
            i += 1
    cal.save("calendar.png")
    return cal

if __name__ == "__main__":
    cal_dict = {"CCST9042": ["1300", "1500"], "CCCH9023": ["1500", "1700"], "CCST9017": ["1700", "1900"], "CCST9018": ["1900", "2100"], "CCST9019": ["2100", "2300"]}
    event = input()
    time1 = input()
    time2 = input()
    print(add_event(cal_dict, event, time1, time2, isEmpty = False))
    draw_calendar(cal_dict)